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

#include <geos/operation/intersection/Rectangle.h>
#include <geos/operation/intersection/RectangleIntersectionBuilder.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/Point.h>
#include <geos/geom/LineString.h>
#include <geos/geom/LinearRing.h>
#include <geos/algorithm/CGAlgorithms.h>
#include <geos/util/IllegalArgumentException.h>

#ifndef GEOS_DEBUG
# define GEOS_DEBUG 0
#endif

namespace {
  struct Trace {
    std::string _name;
    Trace(const std::string& name): _name(name) {
#if GEOS_DEBUG
      std::cout << "--" << _name << " enter" << std::endl;
#endif
    };
    ~Trace() {
#if GEOS_DEBUG
      std::cout << "--" << _name << " exit" << std::endl;
#endif
    };
  };
}

namespace geos {
namespace operation { // geos::operation
namespace intersection { // geos::operation::intersection

using namespace geos::geom;

RectangleIntersectionBuilder::~RectangleIntersectionBuilder()
{
  for (std::list<geom::Polygon *>::iterator i=polygons.begin(), e=polygons.end(); i!=e; ++i)
	  delete *i;
  for (std::list<geom::LineString *>::iterator i=lines.begin(), e=lines.end(); i!=e; ++i)
	  delete *i;
  for (std::list<geom::LineString *>::iterator i=dangling_lines.begin(), e=dangling_lines.end(); i!=e; ++i)
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
  const geom::CoordinateSequence &cs1 = *line1->getCoordinatesRO();

  geom::LineString * line2 = lines.back();
  const geom::CoordinateSequence &cs2 = *line2->getCoordinatesRO();

  const int n1 = cs1.size();
  const int n2 = cs2.size();

  // Safety check against bad input to prevent segfaults
  if(n1==0 || n2==0)
       return;

  if (cs1[0] != cs2[n2-1]) return;

  // Merge the two linestrings

  CoordinateSequence *ncs = CoordinateSequence::removeRepeatedPoints(&cs2);
  ncs->add(&cs1, false, true);

  delete line1;
  delete line2;

  LineString * nline = _gf.createLineString(ncs);
  lines.pop_front();
  lines.pop_back();

  lines.push_front(nline);
}


void RectangleIntersectionBuilder::release(RectangleIntersectionBuilder & theParts)
{
  for (std::list<geom::Polygon *>::iterator i=polygons.begin(), e=polygons.end(); i!=e; ++i)
	  theParts.add(*i);

  for (std::list<geom::LineString *>::iterator i=lines.begin(), e=lines.end(); i!=e; ++i)
	  theParts.add(*i);

  for (std::list<geom::Point *>::iterator i=points.begin(), e=points.end(); i!=e; ++i)
	  theParts.add(*i);

  for (std::list<geom::LineString *>::iterator i=dangling_lines.begin(), e=dangling_lines.end(); i!=e; ++i)
	  theParts.add(*i, true);

  clear();
}

/**
 * \brief Clear the parts having transferred ownership elsewhere
 */

void RectangleIntersectionBuilder::clear()
{
  polygons.clear();
  lines.clear();
  dangling_lines.clear();
  points.clear();
}

/**
 * \brief Test if there are no parts at all
 */

bool RectangleIntersectionBuilder::empty() const
{
  return polygons.empty() && lines.empty() && points.empty() && dangling_lines.empty();
}

size_t
RectangleIntersectionBuilder::size() const
{
  return polygons.size() + lines.size() + points.size() + dangling_lines.size();
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

void
RectangleIntersectionBuilder::add(geom::LineString * theLine, bool dangling)
{
  if ( dangling ) {
    dangling_lines.push_back(theLine);
  } else {
    lines.push_back(theLine);
  }
}

/**
 * \brief Add intermediate Point
 */

void RectangleIntersectionBuilder::add(geom::Point * thePoint)
{
  points.push_back(thePoint);
}

std::auto_ptr<geom::Geometry>
RectangleIntersectionBuilder::build()
{
  // Total number of objects
#if GEOS_DEBUG
  Trace _t("RectangleIntersectionBuilder::build");
#endif

  std::size_t n = size();

  if(n == 0) {
	  return std::auto_ptr<Geometry>(_gf.createGeometryCollection());
  }

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

  for (std::list<geom::LineString *>::iterator i=dangling_lines.begin(), e=dangling_lines.end(); i!=e; ++i)
      geoms->push_back(*i);
  dangling_lines.clear();


  std::auto_ptr<Geometry> ret(
    (*geoms)[0]->getFactory()->buildGeometry(geoms)
  );
  return ret;
}

/**
 * Distance of 1st point of linestring to last point of linearring
 * along rectangle edges
 */

double distance(const Rectangle & rect,
				double x1, double y1,
				double x2, double y2)
{
#if GEOS_DEBUG > 1
  Trace _t("distance");
#endif
  double dist = 0;

  Rectangle::Position pos = rect.position(x1,y1);
  Rectangle::Position endpos = rect.position(x2,y2);

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

double distance(const Rectangle & rect,
				const std::vector<Coordinate> &ring,
				const geom::LineString * line)
{
  double nr = ring.size();
  const Coordinate &c1 = ring[nr-1];

  const CoordinateSequence * linecs = line->getCoordinatesRO();
  const Coordinate &c2 = linecs->getAt(0);

  return distance(rect,c1.x,c1.y,c2.x,c2.y);
}

double distance(const Rectangle & rect,
				const std::vector<Coordinate> &ring)
{
  double nr = ring.size();
  const Coordinate& c1 = ring[nr-1]; // TODO: ring.back() ?
  const Coordinate& c2 = ring[0]; // TODO: ring.front() ?
  return distance(rect, c1.x, c1.y, c2.x, c2.y);
}

/**
 * \brief Reverse given segment in a coordinate vector
 */
void reverse_points(std::vector<Coordinate> &v, int start, int end)
{
  geom::Coordinate p1;
  geom::Coordinate p2;
  while(start < end)
	{
    p1 = v[start];
    p2 = v[end];
    v[start] = p2;
    v[end] = p1;
	  ++start;
	  --end;
	}
}

/**
 * \brief Normalize a ring into lexicographic order
 */
void
normalize_ring(std::vector<Coordinate> &ring)
{
  if(ring.empty())
	return;

  // Find the "smallest" coordinate

  int best_pos = 0;
  int n = ring.size();
  for(int pos = 0; pos<n; ++pos)
	{
    // TODO: use CoordinateLessThan ?
	  if(ring[pos].x < ring[best_pos].x)
		best_pos = pos;
	  else if(ring[pos].x == ring[best_pos].x &&
			  ring[pos].y < ring[best_pos].y )
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

  geom::Coordinate c;
  c = ring[0];
  ring[n-1] = c;
}

void
RectangleIntersectionBuilder::close_boundary(
          const Rectangle & rect,
					std::vector<Coordinate> * ring,
					double x1, double y1,
					double x2, double y2)
{
#if GEOS_DEBUG
  Trace _t("RectangleIntersectionBuilder::close_boundary");
#endif

  Rectangle::Position endpos = rect.position(x2,y2);
  Rectangle::Position pos = rect.position(x1,y1);

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
			ring->push_back(Coordinate(x2,y2));
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

	  ring->push_back(Coordinate(x1,y1));

	}
}

void
RectangleIntersectionBuilder::close_ring(const Rectangle & rect,
				std::vector<Coordinate> * ring)
{
  double nr = ring->size();
  Coordinate &c2 = (*ring)[0];
  Coordinate &c1 = (*ring)[nr-1];
  double x2 = c2.x;
  double y2 = c2.y;
  double x1 = c1.x;
  double y1 = c1.y;

  close_boundary(rect, ring, x1, y1, x2, y2);

}


void
RectangleIntersectionBuilder::reconnectPolygons(const Rectangle & rect, bool cwshell)
{
#if GEOS_DEBUG
  Trace _t("RectangleIntersectionBuilder::reconnectPolygons");
#endif
  // Build the exterior rings first

  typedef std::vector< geom::Geometry *> LinearRingVect;
  typedef std::pair< geom::LinearRing *, LinearRingVect * > ShellAndHoles;
  typedef std::list< ShellAndHoles > ShellAndHolesList;

  ShellAndHolesList exterior;

  // Lines not forming an area will remain as lines
  std::list<geom::LineString *> dangling_lines;

  const CoordinateSequenceFactory &_csf = *_gf.getCoordinateSequenceFactory();

  // If there are no lines, the rectangle must have been
  // inside the exterior ring.
  if(lines.empty() && points.empty())
	{
	  geom::LinearRing * ring = rect.toLinearRing(_gf);
	  exterior.push_back(make_pair(ring, new LinearRingVect()));
	}
  else
	{
    if ( ! cwshell ) reverseLines();

	  // Reconnect all lines into one or more linearrings
	  // using box boundaries if necessary

    std::vector<Coordinate> *ring = NULL;

#if GEOS_DEBUG
    std::cout << lines.size() << " lines to connect" << std::endl;
#endif

	  while(!lines.empty() || ring != NULL)
		{
#if GEOS_DEBUG
    std::cout << "lines.size is " << lines.size() << std::endl;
#endif
		  if(ring == NULL)
			{
			  ring = new std::vector<Coordinate>();
			  LineString *line = lines.front();
			  lines.pop_front();
        line->getCoordinatesRO()->toVector(*ring);
			  delete line;
#if GEOS_DEBUG
        std::cout << "line has " << ring->size() << " coords" << std::endl;
#endif
			}

		  // Distance to own endpoint
		  double own_distance = distance(rect, *ring);
#if GEOS_DEBUG
      std::cout << "own_distance:" << own_distance << std::endl;
#endif

		  // Find line to connect to
		  double best_distance = -1;
		  std::list<LineString*>::iterator best_pos = lines.begin();
		  for(std::list<LineString*>::iterator iter=lines.begin(); iter!=lines.end(); ++iter)
			{
			  double d = distance(rect, *ring, *iter);
			  if(best_distance < 0 || d<best_distance)
				{
				  best_distance = d;
				  best_pos = iter;
				}
			}

#if GEOS_DEBUG
      std::cout << "best_distance:" << best_distance << std::endl;
#endif

		  // If own end point is closest, close the ring and continue
		  if(best_distance < 0 || own_distance < best_distance)
			{
#if GEOS_DEBUG
      std::cout << " own end point is closest" << std::endl;
#endif
        geom::LinearRing *valid_shell = 0;
        // TODO: check ring invalidity before moving on ...
        std::vector<Coordinate> *newring = new std::vector<Coordinate>(*ring);
			  close_ring(rect,newring);
			  normalize_ring(*newring);
        geom::CoordinateSequence *shell_cs = _csf.create(newring);
        try {
          valid_shell = _gf.createLinearRing(shell_cs);
        } catch (const geos::util::IllegalArgumentException& e) {
          // if it's invalid as a ring, it must be a line!
        }
        if ( valid_shell ) {
	        exterior.push_back(make_pair(valid_shell, new LinearRingVect()));
          delete ring;
        } else {
          if ( ! shellCoversRect ) {
            // If shell covers rect we don't keep dangling
            geom::CoordinateSequence *line_cs = _csf.create(ring);
            geom::LineString *line = _gf.createLineString(line_cs);
            dangling_lines.push_back(line);
          }
        }
			  ring = NULL;
			}
		  else
			{
			  LineString * line = *best_pos;
			  int nr = ring->size();
        const CoordinateSequence& cs = *line->getCoordinatesRO();
			  close_boundary(rect, ring,
							 (*ring)[nr-1].x,
							 (*ring)[nr-1].y,
							 cs[0].x,
               cs[0].y);
        // above function adds the 1st point
        for (size_t i=1; i<cs.size(); ++i)
          ring->push_back(cs[i]);
			  //ring->addSubLineString(line,1);
			  delete line;
			  lines.erase(best_pos);
			}
		}

#if GEOS_DEBUG
    std::cout << "all lines closed around rect, ring has " << ( ring ? ring->size() : 0 ) << " coords now" << std::endl;
#endif

	}

  if ( exterior.empty() && shellCoversRect ) {
	  geom::LinearRing * ring = rect.toLinearRing(_gf);
	  exterior.push_back(make_pair(ring, new LinearRingVect()));
  }

  // Attach holes to polygons

  for (std::list<geom::Polygon *>::iterator i=polygons.begin(), e=polygons.end(); i!=e; ++i)
	{
    geom::Polygon *poly = *i;
    const geom::LineString *hole = poly->getExteriorRing();

	  if(exterior.size() == 1)
    {
		  exterior.front().second->push_back( hole->clone() );
    }
	  else
		{
      using geos::algorithm::CGAlgorithms;
		  geom::Coordinate c;
		  hole->getCoordinatesRO()->getAt(0, c);
      for (ShellAndHolesList::iterator i=exterior.begin(), e=exterior.end(); i!=e; ++i)
			{
        ShellAndHoles &p = *i;
        const CoordinateSequence *shell_cs = p.first->getCoordinatesRO();
        if( CGAlgorithms::isPointInRing(c, shell_cs) )
        {
          // add hole to shell
          p.second->push_back(hole->clone());
				  break;
        }
			}
		}

		delete poly;
	}

  // Build the result polygons

  std::list<geom::Polygon *> new_polygons;
  for (ShellAndHolesList::iterator i=exterior.begin(), e=exterior.end(); i!=e; ++i)
  {
    ShellAndHoles &p = *i;
	  geom::Polygon * poly = _gf.createPolygon(p.first, p.second);
	  new_polygons.push_back(poly);
  }

  lines = dangling_lines;
  polygons = new_polygons;
}

void
RectangleIntersectionBuilder::reverseLines()
{
  std::list<geom::LineString *> new_lines;
  for (std::list<geom::LineString *>::reverse_iterator i=lines.rbegin(), e=lines.rend(); i!=e; ++i)
  {
    LineString *ol = *i;
	  new_lines.push_back(dynamic_cast<LineString*>(ol->reverse()));
    delete ol;
  }
  lines = new_lines;
#if GEOS_DEBUG
  std::cout << "After lines reverse, parts are " << *this << std::endl;
#endif
}

std::ostream&
operator<< (std::ostream& os, const RectangleIntersectionBuilder& b)
{
  os << b.points.size() << " points, " << b.lines.size() << " lines, "
     << b.polygons.size() << " polys, "
     << b.dangling_lines.size() << " dangling lines";
  if ( b.shellCoversRect ) os << ", shell covers rect";

  size_t i = 0;
	for(std::list<LineString*>::const_iterator iter=b.lines.begin(); iter!=b.lines.end(); ++iter)
    os << std::endl << "Line " << (i++) << ": " << (*iter)->toString();
  return os;
}

} // namespace geos::operation::intersection
} // namespace geos::operation
} // namespace geos
