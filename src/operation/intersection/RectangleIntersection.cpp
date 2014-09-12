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

#include <geos/algorithm/CGAlgorithms.h>
#include <geos/operation/intersection/RectangleIntersection.h>
#include <geos/operation/intersection/Rectangle.h>
#include <geos/operation/intersection/RectangleIntersectionBuilder.h>
#include <geos/operation/predicate/RectangleIntersects.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/Polygon.h>
#include <geos/geom/MultiPolygon.h>
#include <geos/geom/Point.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/LineString.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Location.h>
#include <geos/util/UnsupportedOperationException.h>
#include <list>
#include <stdexcept>
#include <iostream>

#ifndef GEOS_DEBUG
# define GEOS_DEBUG 0
#endif

using geos::operation::intersection::Rectangle;
using geos::operation::intersection::RectangleIntersectionBuilder;
using namespace geos::geom;

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
};

namespace geos {
namespace operation { // geos::operation
namespace intersection { // geos::operation::intersection

/**
 * \brief Test if two coordinates are different
 */

inline
bool different(double x1, double y1, double x2, double y2)
{
  return !(x1==x2 && y1==y2);
}

/**
 * \brief Calculate a line intersection point
 *
 * Note:
 *   - Calling this with x1,y1 and x2,y2 swapped cuts the other end of the line
 *   - Calling this with x and y swapped cuts in y-direction instead
 *   - Calling with 1<->2 and x<->y swapped works too
 */

inline
void clip_one_edge(double & x1, double & y1, double x2, double y2, double limit)
{
  if(x1 != x2)
	{
	  y1 += (y2-y1)*(limit-x1)/(x2-x1);
	  x1 = limit;
	}
}

/**
 * \brief Start point is outside, endpoint is definitely inside
 *
 * Note: Even though one might think using >= etc operators would produce
 *       the same result, that is not the case. We rely on the fact
 *       that nothing is clipped unless the point is truly outside the
 *       rectangle! Without this handling lines ending on the edges of
 *       the rectangle would be very difficult.
 */

void clip_to_edges(double & x1, double & y1,
				   double x2, double y2,
				   const Rectangle & rect)
{
  if(x1 < rect.xmin())
	clip_one_edge(x1,y1,x2,y2,rect.xmin());
  else if(x1 > rect.xmax())
	clip_one_edge(x1,y1,x2,y2,rect.xmax());

  if(y1<rect.ymin())
	clip_one_edge(y1,x1,y2,x2,rect.ymin());
  else if(y1>rect.ymax())
	clip_one_edge(y1,x1,y2,x2,rect.ymax());
}


/**
 * \brief Clip  geometry
 *
 * Here outGeom may also be a MultiPoint
 */

void
RectangleIntersection::clip_point(const geom::Point * g,
				RectangleIntersectionBuilder & parts,
				const Rectangle & rect)
{
  if(g == NULL)
	return;
  
  double x = g->getX();
  double y = g->getY();
  
  if(rect.position(x,y) == Rectangle::Inside)
	parts.add(dynamic_cast<geom::Point*>(g->clone()));
}

bool
RectangleIntersection::clip_linestring_parts(const geom::LineString * gi,
						   RectangleIntersectionBuilder & parts,
						   const Rectangle & rect)
{
  using namespace geos::geom;

#if GEOS_DEBUG
  Trace _t("RectangleIntersection::clip_linestring_parts");
#endif

  int n = gi->getNumPoints();

  if(gi == NULL || n<1)
	return false;

  // For shorthand code

  std::vector<Coordinate> cs;
  gi->getCoordinatesRO()->toVector(cs);

  // Keep a record of the point where a line segment entered
  // the rectangle. If the boolean is set, we must insert
  // the point to the beginning of the linestring which then
  // continues inside the rectangle.

  double x0 = 0;
  double y0 = 0;
  bool add_start = false;

  // Start iterating

  int i = 0;

  while(i<n)
	{
	  // Establish initial position

	  double x = cs[i].x;
	  double y = cs[i].y;
	  Rectangle::Position pos = rect.position(x,y);

	  if(pos == Rectangle::Outside)
		{
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is outside" << std::endl;
#endif

		  // Skip points as fast as possible until something has to be checked
		  // in more detail.

		  ++i;	// we already know it is outside

		  if(x < rect.xmin())
			while(i < n && cs[i].x < rect.xmin())
			  ++i;

		  else if(x > rect.xmax())
			while(i < n && cs[i].x > rect.xmax())
			  ++i;

		  else if(y < rect.ymin())
			while(i < n && cs[i].y < rect.ymin())
			  ++i;

		  else if(y > rect.ymax())
			while(i < n && cs[i].y > rect.ymax())
			  ++i;

		  if(i >= n)
			return false; // fully outside

		  // Establish new position
		  x = cs[i].x;
		  y = cs[i].y;
		  pos = rect.position(x,y);

		  // Handle all possible cases
		  x0 = cs[i-1].x;
		  y0 = cs[i-1].y;
		  clip_to_edges(x0,y0,x,y,rect);

#if GEOS_DEBUG
      std::cout << "P " << (i-1) << ": " << cs[i-1] << " was last outside" << std::endl;
#endif

		  if(pos == Rectangle::Inside)
			{
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is inside (out to in)" << std::endl;
#endif
			  add_start = true;	// x0,y0 must have clipped the rectangle
			  // Main loop will enter the Inside/Edge section

			}
		  else if(pos == Rectangle::Outside)
			{
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is outside (out to out)" << std::endl;
#endif
			  // From Outside to Outside. We need to check whether
			  // we created a line segment inside the box. In any
			  // case, we will continue the main loop after this
			  // which will then enter the Outside section.

			  // Clip the other end too
			  clip_to_edges(x,y,x0,y0,rect);

			  Rectangle::Position prev_pos = rect.position(x0,y0);
			  pos = rect.position(x,y);

			  if( Rectangle::onEdge(prev_pos) &&		// discard if does not intersect rect
				  Rectangle::onEdge(pos) &&
				  !Rectangle::onSameEdge(prev_pos,pos)	// discard if travels along edge
				  )
				{
			    if( different(x0,y0,x,y) ) {
            std::vector<Coordinate> *coords = new std::vector<Coordinate>(2);
            (*coords)[0] = Coordinate(x0,y0);
            (*coords)[1] = Coordinate(x,y);
            CoordinateSequence *seq = _csf->create(coords);
            geom::LineString * line = _gf->createLineString(seq);
            parts.add(line);
          }
#if 0
          else {
std::cout << " Adding point!" << std::endl;
            geom::Point *point = _gf->createPoint(Coordinate(x0, y0));
            parts.add(point);
          }
#endif
				}

			  // Continue main loop outside the rect

			}
		  else
			{
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is edge (out to edge)" << std::endl;
#endif
			  // From outside to edge. If the edge we hit first when
			  // following the line is not the edge we end at, then
			  // clearly we must go through the rectangle and hence
			  // a start point must be set.

			  Rectangle::Position newpos = rect.position(x0,y0);
			  if(!Rectangle::onSameEdge(pos,newpos))
				{
				  add_start = true;
				}
			  else
				{
				  // we ignore the travel along the edge and continue
				  // the main loop at the last edge point
				}
			}
		}

	  else // inside or edge
		{

#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is " << ( Rectangle::onEdge(pos) ? "on edge" : "inside" )
                << " -- start_index:" << i << std::endl;
#endif

		  // The point is now stricly inside or on the edge.
		  // Keep iterating until the end or the point goes
		  // outside. We may have to output partial linestrings
		  // while iterating until we go strictly outside

		  int start_index = i;			// 1st valid original point
			Rectangle::Position start_pos = pos;
		  bool go_outside = false;

		  while(!go_outside && ++i<n)
			{
			  x = cs[i].x;
			  y = cs[i].y;

			  Rectangle::Position prev_pos = pos;
			  pos = rect.position(x,y);

			  //if(pos == prev_pos)
			  if(Rectangle::onSameEdge(prev_pos,pos))
				{
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " on same edge, keep going " << std::endl;
#endif
				  // Just keep going
				}
			  else if(pos == Rectangle::Inside)
				{
          if ( ! Rectangle::onEdge(prev_pos) ) {
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is inside (from inside), keep going" << std::endl;
#endif
          } else {
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is inside (from edge), close edge line" << std::endl;
      std::cout << " start_index:" << start_index << " i:" << i << std::endl;
#endif
            // from edge to inside, add line or point or whatever
            if(start_index < i-1) {
              std::vector<Coordinate> *coords = new std::vector<Coordinate>();
              coords->insert(coords->end(), cs.begin()+start_index, cs.begin()+i);
              CoordinateSequence *seq = _csf->create(coords);
              geom::LineString * line = _gf->createLineString(seq);
              // this is a boundary line
              parts.add(line, true); // boundary line !
#if GEOS_DEBUG
              std::cout << " boundary line added, parts become " << parts << std::endl;
              std::cout << " added line is " << line->toString() << std::endl;
#endif
            }
            start_index = i-1;
            start_pos = pos;
          }
				}
			  else if(pos == Rectangle::Outside)
				{
#if GEOS_DEBUG
      std::cout << "P " << i << ": " << cs[i] << " is outside" << std::endl;
#endif
				  go_outside = true;

				  // Clip the outside point to edges
				  clip_to_edges(x, y, cs[i-1].x, cs[i-1].y, rect);
				  pos = rect.position(x,y);

				  // Does the line exit through the inside of the box?

				  bool through_box = (different(x,y,cs[i].x,cs[i].y) &&
									  !Rectangle::onSameEdge(prev_pos,pos));

#if GEOS_DEBUG
      std::cout << " through_box:" << through_box << " add_start:" << add_start << " start_index:" << start_index << " i:" << i << std::endl;
#endif

				  // Output a LineString if it at least one segment long

				  if(start_index < i-1 || add_start || through_box)
					{
            std::vector<Coordinate> *coords = new std::vector<Coordinate>();
					  if(add_start)
						{
						  coords->push_back(Coordinate(x0, y0));
						  add_start = false;
						}
					  //line->addSubLineString(&g, start_index, i-1);
            coords->insert(coords->end(), cs.begin()+start_index, cs.begin()+i);

					  if(through_box) coords->push_back(Coordinate(x,y));

            CoordinateSequence *seq = _csf->create(coords);
            geom::LineString * line = _gf->createLineString(seq);

            // TODO: check if this was a boundary line !
					  parts.add(line);
#if GEOS_DEBUG
            std::cout << " line added (X), parts become " << parts << std::endl;
            std::cout << " added line is " << line->toString() << std::endl;
#endif
					}
				  // Output a Point if clipped segment was a point 
          else if ( x == cs[i-1].x && y == cs[i-1].y )
          {
#if 1
//std::cout << " Adding point!" << std::endl;
            geom::Point *point = _gf->createPoint(cs[i-1]);
            parts.add(point);
#endif
          }
				  // And continue main loop on the outside
				}
			  else
				{

#if GEOS_DEBUG
          std::cout << "P " << i << ": " << cs[i] << " is on edge (from " << ( Rectangle::onEdge(start_pos) ? "other edge" : "inside" ) << ")" << std::endl;
          std::cout << " start_index:" << start_index << " i:" << i << " add_start:" << add_start << std::endl;
#endif

          // close boundary edge first
				  if(start_index < i-1 && Rectangle::onEdge(start_pos)) {
            std::vector<Coordinate> *coords = new std::vector<Coordinate>();
            coords->insert(coords->end(), cs.begin()+start_index, cs.begin()+i);
            CoordinateSequence *seq = _csf->create(coords);
            geom::LineString * line = _gf->createLineString(seq);
            parts.add(line, true); // boundary line
#if GEOS_DEBUG
          std::cout << " boundary line added (XX), parts become " << parts << std::endl;
          std::cout << " added line is " << line->toString() << std::endl;
#endif
            start_index = i-1;
            start_pos = prev_pos;
          }

          std::vector<Coordinate> *coords = new std::vector<Coordinate>();
          if(add_start)
          {
            coords->push_back(Coordinate(x0, y0));
            add_start = false;
          }
          coords->insert(coords->end(), cs.begin()+start_index, cs.begin()+i+1);
#if GEOS_DEBUG
          //std::cout << "start_index to i makes a " << coords->size() << " array" << std::endl;
#endif

          // TODO: check if this was a boundary line !
          CoordinateSequence *seq = _csf->create(coords);
          geom::LineString * line = _gf->createLineString(seq);
          parts.add(line);
#if GEOS_DEBUG
          std::cout << " line added (XX), parts become " << parts << std::endl;
          std::cout << " added line is " << line->toString() << std::endl;
#endif
          start_index = i;
          start_pos = pos;
#if GEOS_DEBUG
          std::cout << " start_index updated to " << start_index << std::endl;
#endif
				}
			}

		  // Was everything in?
		  // If so, generate no output but return true in this case only
		  // TODO: review this for holes laying on a boundary !
		  if(start_index == 0 && i >= n)
      {
#if GEOS_DEBUG
        std::cout << "all inside!" << parts << std::endl;
#endif
			  return true;
      }

		  // Flush the last line segment if data ended and there is something to flush

		  if(!go_outside &&						// meaning data ended
			 (start_index < i-1 || add_start))	// meaning something has to be generated
			{
        std::vector<Coordinate> *coords = new std::vector<Coordinate>();
			  //geom::LineString * line = new geom::LineString();
			  if(add_start)
				{
				  //line->addPoint(x0,y0);
					coords->push_back(Coordinate(x0, y0));
				  add_start = false;
				}
			  //line->addSubLineString(&g, start_index, i-1);
        coords->insert(coords->end(), cs.begin()+start_index, cs.begin()+i);

        CoordinateSequence *seq = _csf->create(coords);
        geom::LineString * line = _gf->createLineString(seq);
			  parts.add(line);
#if GEOS_DEBUG
        std::cout << " line added, parts become " << parts << std::endl;
        std::cout << " added line is " << line->toString() << std::endl;
#endif
			}

		}
	}

  return false;

}

/**
 * \brief Clip polygon, do not close clipped ones
 */

void
RectangleIntersection::clip_polygon_to_linestrings(const geom::Polygon * g,
								 RectangleIntersectionBuilder & toParts,
								 const Rectangle & rect)
{
  if(g == NULL || g->isEmpty())
	return;

  // Clip the exterior first to see what's going on

  RectangleIntersectionBuilder parts(*_gf);

  // If everything was in, just clone the original

  if(clip_linestring_parts(g->getExteriorRing(), parts, rect))
	{
	  toParts.add(dynamic_cast<geom::Polygon *>(g->clone()));
	  return;
	}

  // Now, if parts is empty, our rectangle may be inside the polygon
  // If not, holes are outside too

  if(parts.empty())
	{
	  // We could now check whether the rectangle is inside the outer
	  // ring to avoid checking the holes. However, if holes are much
	  // smaller than the exterior ring just checking the holes
	  // separately could be faster.

	  if(g->getNumInteriorRing() == 0)
		return;

	}
  else
	{
	  // The exterior must have been clipped into linestrings.
	  // Move them to the actual parts collector, clearing parts.

	  parts.reconnect();
	  parts.release(toParts);
	}

  // Handle the holes now:
  // - Clipped ones become linestrings
  // - Intact ones become new polygons without holes

  for(int i=0, n=g->getNumInteriorRing(); i<n; ++i)
	{
	  if(clip_linestring_parts(g->getInteriorRingN(i), parts, rect))
		{
#if GEOS_DEBUG
      std::cout << "Hole " << i << " completely wraps the rect" << std::endl;
#endif
      // clones
		  LinearRing *hole = dynamic_cast<LinearRing*>(g->getInteriorRingN(i)->clone());
      // becomes exterior
		  Polygon *poly = _gf->createPolygon(hole, 0);
		  toParts.add(poly);
		}
	  else if(!parts.empty())
		{
		  parts.reconnect();
		  parts.release(toParts);
		}
	}
}

/**
 * \brief Clip polygon, close clipped ones
 */

void
RectangleIntersection::clip_polygon_to_polygons(const geom::Polygon * g,
							  RectangleIntersectionBuilder & toParts,
							  const Rectangle & rect)
{
#if GEOS_DEBUG
  Trace _t("RectangleIntersection::clip_polygon_to_polygons");
#endif

  if(g == NULL || g->isEmpty())
	return;

  // Clip the exterior first to see what's going on

  RectangleIntersectionBuilder parts(*_gf);

  // If everything was in, just clone the original

#if GEOS_DEBUG
  std::cout << "clip shell" << std::endl;
#endif
  if(clip_linestring_parts(g->getExteriorRing(), parts, rect))
	{
	  toParts.add(dynamic_cast<geom::Polygon *>(g->clone()));
	  return;
	}

  // If there were no intersections, the outer ring might be
  // completely outside.

  using geos::algorithm::CGAlgorithms;
  if( parts.empty() )
  {
    const Coordinate rectCorner(rect.xmin(), rect.ymin());
    if ( CGAlgorithms::locatePointInRing(rectCorner,
                          *g->getExteriorRing()->getCoordinatesRO())
         != Location::INTERIOR )
    {
      return; // return completely outside ?
    }
    else
    {
      // fully wraps the rectangle
	    toParts.shellCoversRect = parts.shellCoversRect = true; 
    }
  }

  // Must do this to make sure all end points are on the edges

  parts.reconnect();

#if GEOS_DEBUG
  std::cout << "After shell parte reconnect, parts are " << parts << std::endl;
#endif

  // Handle the holes now:
  // - Clipped ones become part of the exterior
  // - Intact ones become holes in new polygons formed by exterior parts

  for(int i=0, n=g->getNumInteriorRing(); i<n; ++i)
	{
	  RectangleIntersectionBuilder holeparts(*_gf);
#if GEOS_DEBUG
  std::cout << "clip hole " << i << std::endl;
#endif
	  if(clip_linestring_parts(g->getInteriorRingN(i), holeparts, rect))
		{
#if GEOS_DEBUG
      std::cout << "Hole " << i << " full inside " << std::endl;
#endif
      // clones
		  LinearRing *hole = dynamic_cast<LinearRing*>(g->getInteriorRingN(i)->clone());
      // becomes exterior
		  Polygon *poly = _gf->createPolygon(hole, 0);
		  parts.add(poly);
#if GEOS_DEBUG
      std::cout << " polygon added, parts become " << parts << std::endl;
#endif
		}
	  else
		{
#if GEOS_DEBUG
      std::cout << "Hole " << i << " parts are " << holeparts << std::endl;
#endif
		  if(!holeparts.empty())
			{
			  holeparts.reconnect();
#if GEOS_DEBUG
      std::cout << "after reconnect, hole " << i << " parts are " << holeparts << std::endl;
#endif
			  holeparts.release(parts);
			}
		  else
			{
        using geos::algorithm::CGAlgorithms;
			  if( CGAlgorithms::isPointInRing(Coordinate(rect.xmin(), rect.ymin()),
            g->getInteriorRingN(i)->getCoordinatesRO()) )
				{
#if GEOS_DEBUG
      std::cout << "Hole " << i << " completely wraps the rect" << std::endl;
#endif
				  // Completely inside the hole
				  return;
				}
			}
		}
	}

#if GEOS_DEBUG
  std::cout << "by the end of clip_polygon_to_polygons, parts are " << parts << std::endl;
#endif

  parts.reconnectPolygons(rect);

  parts.release(toParts);

}

/**
 * \brief Clip  geometry
 */

void
RectangleIntersection::clip_polygon(const geom::Polygon * g,
				  RectangleIntersectionBuilder & parts,
				  const Rectangle & rect,
				  bool keep_polygons)
{
  if(keep_polygons)
	clip_polygon_to_polygons(g, parts, rect);
  else
	clip_polygon_to_linestrings(g, parts, rect);
}

/**
 * \brief Clip geometry
 */

void
RectangleIntersection::clip_linestring(const geom::LineString * g,
					 RectangleIntersectionBuilder & parts,
					 const Rectangle & rect)
{
  if(g == NULL || g->isEmpty())
	return;

  // If everything was in, just clone the original

  if(clip_linestring_parts(g, parts, rect))
	parts.add(dynamic_cast<geom::LineString *>(g->clone()));
#if GEOS_DEBUG
  std::cout << " line added, parts become " << parts << std::endl;
#endif
  
}

void
RectangleIntersection::clip_multipoint(const geom::MultiPoint * g,
					 RectangleIntersectionBuilder & parts,
					 const Rectangle & rect)
{
  if(g == NULL || g->isEmpty())
	return;
  for(int i=0, n=g->getNumGeometries(); i<n; ++i)
	{
	  clip_point(dynamic_cast<const geom::Point *>(g->getGeometryN(i)),
				 parts, rect);
	}
}

void
RectangleIntersection::clip_multilinestring(const geom::MultiLineString * g,
						  RectangleIntersectionBuilder & parts,
						  const Rectangle & rect)
{
  if(g == NULL || g->isEmpty())
	return;
  
  for(int i=0, n=g->getNumGeometries(); i<n; ++i)
	{
	  clip_linestring(dynamic_cast<const geom::LineString *>(g->getGeometryN(i)),
					  parts, rect);
	}
}

void
RectangleIntersection::clip_multipolygon(const geom::MultiPolygon * g,
					   RectangleIntersectionBuilder & parts,
					   const Rectangle & rect,
					   bool keep_polygons)
{
  if(g == NULL || g->isEmpty())
	return;

  for(int i=0, n=g->getNumGeometries(); i<n; ++i)
	{
	  clip_polygon(dynamic_cast<const geom::Polygon *>(g->getGeometryN(i)),
				   parts, rect, keep_polygons);
	}
}

void
RectangleIntersection::clip_geometrycollection(
               const geom::GeometryCollection * g,
							 RectangleIntersectionBuilder & parts,
							 const Rectangle & rect,
							 bool keep_polygons)
{
  if(g == NULL || g->isEmpty())
	return;
  
  for(int i=0, n=g->getNumGeometries(); i<n; ++i)
	{
	  clip_geom(g->getGeometryN(i),
				parts, rect, keep_polygons);
	}
}

void
RectangleIntersection::clip_geom(const geom::Geometry * g,
			   RectangleIntersectionBuilder & parts,
			   const Rectangle & rect,
			   bool keep_polygons)
{
	if ( const Point* p = dynamic_cast<const geom::Point *>(g) )
	  return clip_point(p, parts, rect);
	else if ( const MultiPoint* p = dynamic_cast<const geom::MultiPoint *>(g) )
	  return clip_multipoint(p, parts, rect);
	else if ( const LineString* p = dynamic_cast<const geom::LineString *>(g) )
	  return clip_linestring(p, parts, rect);
	else if ( const MultiLineString* p = dynamic_cast<const geom::MultiLineString *>(g) )
	  return clip_multilinestring(p, parts, rect);
	else if ( const Polygon* p = dynamic_cast<const geom::Polygon *>(g) )
	  return clip_polygon(p, parts, rect, keep_polygons);
	else if ( const MultiPolygon* p = dynamic_cast<const geom::MultiPolygon *>(g) )
	  return clip_multipolygon(p, parts, rect, keep_polygons);
	else if ( const GeometryCollection* p = dynamic_cast<const geom::GeometryCollection *>(g) )
	  return clip_geometrycollection(p, parts, rect, keep_polygons);
  else {
    throw util::UnsupportedOperationException("Encountered an unknown geometry component when clipping polygons");
  }
}

/* public static */
std::auto_ptr<geom::Geometry>
RectangleIntersection::clipBoundary(const geom::Geometry & g, const Rectangle & rect)
{
  RectangleIntersection ri(g,rect);
  return ri.clipBoundary();
}

std::auto_ptr<geom::Geometry>
RectangleIntersection::clipBoundary()
{
  RectangleIntersectionBuilder parts(*_gf);

  bool keep_polygons = false;
  clip_geom(&_geom, parts, _rect, keep_polygons);

  return parts.build();
}

/* public static */
std::auto_ptr<geom::Geometry>
RectangleIntersection::clip(const geom::Geometry & g, const Rectangle & rect)
{
  RectangleIntersection ri(g,rect);
  return ri.clip();
}

std::auto_ptr<geom::Geometry>
RectangleIntersection::clip()
{
  RectangleIntersectionBuilder parts(*_gf);

  bool keep_polygons = true;
  clip_geom(&_geom, parts, _rect, keep_polygons);

  return parts.build();
}

RectangleIntersection::RectangleIntersection(const geom::Geometry& geom, const Rectangle& rect)
  : _geom(geom), _rect(rect),
    _gf(geom.getFactory())
{
  _csf = _gf->getCoordinateSequenceFactory();
}

} // namespace geos::operation::intersection
} // namespace geos::operation
} // namespace geos
