/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2014 Mika Heiskanen <mika.heiskanen@fmi.fi>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation. 
 * See the COPYING file for more information.
 *
 **********************************************************************/

#include <geos/operation/intersection/RectangleIntersectionBuilder.h>

RectangleIntersectionBuilder::~RectangleIntersectionBuilder()
{
  for (std::list<geom::Polygon *>::iterator i=polygons.begin(), e=polygons.end(); i!=e; ++i)
	  delete *i;
  for (std::list<geom::LineString *>::iterator i=lines.begin(), e=lines.end(); i!=e; ++i)
	  delete *i;
  for (std::list<geom::Point *>::iterator i=points.begin(), e=points.end(); i!=e; ++i)
	  delete *i;
}

void
RectangleIntersectionBuilder::reconnect()
{
  // Nothing to reconnect if there aren't at least two lines
  if(lines.size() < 2)
	return;

  geom::LineString * line1 = lines.front();
  geom::LineString * line2 = lines.back();

  const int n1 = line1->getNumPoints();
  const int n2 = line2->getNumPoints();

  // Safety check against bad input to prevent segfaults
  if(n1==0 || n2==0)
	return;

  if(line1->getX(0) != line2->getX(n2-1) ||
	 line1->getY(0) != line2->getY(n2-1))
	{
	  return;
	}

  // Merge the two linestrings

  line2->addSubLineString(line1,1,n1-1);
  delete line1;
  lines.pop_front();
  lines.pop_back();
  lines.push_front(line2);

}


void RectangleIntersectionBuilder::release(RectangleIntersectionBuilder & theParts)
{
  for (std::list<geom::Polygon *>::iterator i=polygons.begin(), e=polygons.end(); i!=e; ++i)
	  theParts.add(*i);

  for (std::list<geom::LineString *>::iterator i=lines.begin(), e=lines.end(); i!=e; ++i)
	  theParts.add(*i);

  for (std::list<geom::Point *>::iterator i=points.begin(), e=points.end(); i!=e; ++i)
	  theParts.add(*i);

  clear();
}

/**
 * \brief Clear the parts having transferred ownership elsewhere
 */

void RectangleIntersectionBuilder::clear()
{
  polygons.clear();
  lines.clear();
  points.clear();
}

/**
 * \brief Test if there are no parts at all
 */

bool RectangleIntersectionBuilder::empty() const
{
  return polygons.empty() && lines.empty() && points.empty();
}

/**
 * \brief Add intermediate Polygon
 */

void RectangleIntersectionBuilder::add(geom::Polygon * thePolygon)
{
  polygons.push_back(thePolygon);
}

/**
 * \brief Add intermediate LineString
 */

void RectangleIntersectionBuilder::add(geom::LineString * theLine)
{
  lines.push_back(theLine);
}

/**
 * \brief Add intermediate Point
 */

void RectangleIntersectionBuilder::add(geom::Point * thePoint)
{
  points.push_back(thePoint);
}

/**
 * \brief Build the result geometry from partial results and clean up
 */

geom::Geometry * RectangleIntersectionBuilder::build()
{
  auto * ptr = internalBuild();
  clear();
  return ptr;
}

/**
 * \brief Build the result geometry from the partial results
 *
 * Does NOT clear the used data!
 */

geom::Geometry * RectangleIntersectionBuilder::internalBuild() const
{
  // Total number of objects

  std::size_t n = polygons.size() + lines.size() + points.size();

  if(n == 0)
	return new geom::GeometryCollection; // TODO: GeometryFactory!!

  std::vector<Geometry *> *geoms = new std::vector<Geometry *>;
  geoms->reserve(n);

  for (std::list<geom::Polygon *>::iterator i=polygons.begin(), e=polygons.end(); i!=e; ++i)
      geoms->push_back(*i);
  polygons.clear();

  for (std::list<geom::LineString *>::iterator i=lines.begin(), e=lines.end(); i!=e; ++i)
      geoms->push_back(*i);
  lines.clear();

  for (std::list<geom::Point *>::iterator i=points.begin(), e=points.end(); i!=e; ++i)
      geoms->push_back(*i);
  points.clear();

  return (*geoms)[0]->getFactory()->buildGeometry(geoms);
}

/**
 * Distance of 1st point of linestring to last point of linearring
 * along rectangle edges
 */

double distance(const Rectangle & rect,
				double x1, double y1,
				double x2, double y2)
{
  double dist = 0;

  auto pos = rect.position(x1,y1);
  auto endpos = rect.position(x2,y2);

  while(true)
	{
	  // Close up when we have the same edge and the
	  // points are in the correct clockwise order
	  if((pos & endpos) != 0 &&
		 (
		  (x1==rect.xmin() && y2>=y1) ||
		  (y1==rect.ymax() && x2>=x1) ||
		  (x1==rect.xmax() && y2<=y1) ||
		  (y1==rect.ymin() && x2<=x1))
		  )
		{
		  dist += fabs(x2-x1) + fabs(y2-y1);
		  break;
		}

	  pos = Rectangle::nextEdge(pos);
	  if(pos & Rectangle::Left)
		{
		  dist += x1-rect.xmin();
		  x1 = rect.xmin();
		}
	  else if(pos & Rectangle::Top)
		{
		  dist += rect.ymax()-y1;
		  y1 = rect.ymax();
		}
	  else if(pos & Rectangle::Right)
		{
		  dist += rect.xmax()-x1;
		  x1 = rect.xmax();
		}
	  else
		{
		  dist += y1-rect.ymin();
		  y1 = rect.ymin();
		}
	}
  return dist;
}

double distance(const Rectangle:: & rect,
				geom::LinearRing * ring,
				geom::LineString * line)
{
  double nr = ring->getNumPoints();
  double x1 = ring->getX(nr-1);
  double y1 = ring->getY(nr-1);

  double x2 = line->getX(0);
  double y2 = line->getY(0);

  return distance(rect,x1,y1,x2,y2);
}

double distance(const Rectangle:: & rect,
				geom::LinearRing * ring)
{
  double nr = ring->getNumPoints();
  return distance(rect,
				  ring->getX(nr-1),ring->getY(nr-1),
				  ring->getX(0),ring->getY(0));
}

/**
 * \brief Reverse given segment in a linestring
 */

void reverse_points(geom::LineString * line, int start, int end)
{
  geom::Point p1;
  geom::Point p2;
  while(start < end)
	{
	  line->getPoint(start,&p1);
	  line->getPoint(end  ,&p2);
	  line->setPoint(start,&p2);
	  line->setPoint(end  ,&p1);
	  ++start;
	  --end;
	}
}

/**
 * \brief Normalize a ring into lexicographic order
 */

void normalize_ring(geom::LinearRing * ring)
{
  if(ring->IsEmpty())
	return;

  // Find the "smallest" coordinate

  int best_pos = 0;
  int n = ring->getNumPoints();
  for(int pos = 0; pos<n; ++pos)
	{
	  if(ring->getX(pos) < ring->getX(best_pos))
		best_pos = pos;
	  else if(ring->getX(pos) == ring->getX(best_pos) &&
			  ring->getY(pos) < ring->getY(best_pos) )
		best_pos = pos;
	}

  // Quick exit if the ring is already normalized
  if(best_pos == 0)
	return;

  // Flip hands -algorithm to the part without the
  // duplicate last coordinate at n-1:

  reverse_points(ring,0,best_pos-1);
  reverse_points(ring,best_pos,n-2);
  reverse_points(ring,0,n-2);

  // And make sure the ring is valid by duplicating the first coordinate
  // at the end:

  geom::Point point;
  ring->getPoint(0,&point);
  ring->setPoint(n-1,&point);

}

/**
 * \brief Close a ring clockwise along rectangle edges
 *
 * Only the 4 corners and x1,y1 need to be considered. The possible
 * cases are:
 *
 *    x1,y1
 *    corner1 x1,y1
 *    corner1 corner2 x1,y1
 *    corner1 corner2 corner3 x1,y1
 *    corner1 corner2 corner3 corner4 x1,y1
 */

void close_boundary(const Rectangle:: & rect,
					geom::LinearRing * ring,
					double x1, double y1,
					double x2, double y2)
{
  auto endpos = rect.position(x2,y2);
  auto pos = rect.position(x1,y1);

  while(true)
	{
	  // Close up when we have the same edge and the
	  // points are in the correct clockwise order
	  if((pos & endpos) != 0 &&
		 (
		  (x1==rect.xmin() && y2>=y1) ||
		  (y1==rect.ymax() && x2>=x1) ||
		  (x1==rect.xmax() && y2<=y1) ||
		  (y1==rect.ymin() && x2<=x1))
		  )
		{
		  if(x1!=x2 || y1!=y2)		// the polygon may have started at a corner
			ring->addPoint(x2,y2);
		  break;
		}

	  pos = Rectangle::nextEdge(pos);
	  if(pos & Rectangle::Left)
		x1 = rect.xmin();
	  else if(pos & Rectangle::Top)
		y1 = rect.ymax();
	  else if(pos & Rectangle::Right)
		x1 = rect.xmax();
	  else
		y1 = rect.ymin();

	  ring->addPoint(x1,y1);

	}
}

void close_ring(const Rectangle:: & rect,
				geom::LinearRing * ring)
{
  double nr = ring->getNumPoints();
  double x2 = ring->getX(0);
  double y2 = ring->getY(0);
  double x1 = ring->getX(nr-1);
  double y1 = ring->getY(nr-1);

  close_boundary(rect, ring, x1, y1, x2, y2);

}

/**
 * \brief Build polygons from parts left by clipping one
 *
 * 1. Build exterior ring(s) from lines
 * 2. Attach polygons as holes to the exterior ring(s)
 */

void RectangleIntersectionBuilder::reconnectPolygons(const Rectangle & rect)
{
  // Build the exterior rings first

  std::list<geom::LinearRing *> exterior;

  // If there are no lines, the rectangle must have been
  // inside the exterior ring.

  if(lines.empty())
	{
	  geom::LinearRing * ring = new geom::LinearRing;
	  ring->addPoint(rect.xmin(), rect.ymin());
	  ring->addPoint(rect.xmin(), rect.ymax());
	  ring->addPoint(rect.xmax(), rect.ymax());
	  ring->addPoint(rect.xmax(), rect.ymin());
	  ring->addPoint(rect.xmin(), rect.ymin());
	  exterior.push_back(ring);
	}
  else
	{
	  // Reconnect all lines into one or more linearrings
	  // using box boundaries if necessary

	  geom::LinearRing * ring = NULL;

	  while(!lines.empty() || ring != NULL)
		{
		  if(ring == NULL)
			{
			  ring = new geom::LinearRing;
			  auto * line = lines.front();
			  lines.pop_front();
			  ring->addSubLineString(line);
			  delete line;
			}

		  // Distance to own endpoint
		  double own_distance = distance(rect,ring);

		  // Find line to connect to
		  double best_distance = -1;
		  auto best_pos = lines.begin();
		  for(auto iter=lines.begin(); iter!=lines.end(); ++iter)
			{
			  double d = distance(rect, ring, *iter);
			  if(best_distance < 0 || d<best_distance)
				{
				  best_distance = d;
				  best_pos = iter;
				}
			}

		  // If own end point is closest, close the ring and continue
		  if(best_distance < 0 || own_distance < best_distance)
			{
			  close_ring(rect,ring);
			  normalize_ring(ring);
			  exterior.push_back(ring);
			  ring = NULL;
			}
		  else
			{
			  auto * line = *best_pos;
			  int nr = ring->getNumPoints();
			  close_boundary(rect,ring,
							 ring->getX(nr-1),ring->getY(nr-1),
							 line->getX(0),line->getY(0));
			  ring->addSubLineString(line,1);	// above function adds the 1st point
			  delete line;
			  lines.erase(best_pos);
			}
		}
	}

  // Build the result polygons

  std::list<geom::Polygon *> new_polygons;
  BOOST_FOREACH(auto * ring, exterior)
	{
	  geom::Polygon * poly = new geom::Polygon;
	  poly->addRingDirectly(ring);
	  new_polygons.push_back(poly);
	}

  // Attach holes to polygons

  BOOST_FOREACH(auto * hole, polygons)
	{
	  if(new_polygons.size() == 1)
		new_polygons.front()->addRing(hole->getExteriorRing());
	  else
		{
		  geom::Point point;
		  hole->getExteriorRing()->getPoint(0,&point);
		  BOOST_FOREACH(auto * poly, new_polygons)
			{
			  if(poly->getExteriorRing()->isPointInRing(&point,false))
				{
				  poly->addRing(hole->getExteriorRing());
				  break;
				}
			}
		}
	  delete hole;
	}

  clear();
  polygons = new_polygons;
}
