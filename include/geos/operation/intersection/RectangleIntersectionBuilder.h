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

#ifndef GEOS_OP_INTERSECTION_RECTANGLEINTERSECTIONBUILDER_H
#define GEOS_OP_INTERSECTION_RECTANGLEINTERSECTIONBUILDER_H

#include <geos/export.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251) // warning C4251: needs to have dll-interface to be used by clients of class
#endif

#include <list>

// Forward declarations
namespace geos {
  namespace geom {
	class Polygon;
	class LineString;
	class Point;
  }
  namespace operation {
	namespace intersection {
	  class Rectangle;
	}
  }
}

namespace geos {
namespace operation { // geos::operation
namespace intersection { // geos::operation::intersection

/**
 * \brief Rebuild geometries from subpaths left by clipping with a rectangle
 *
 * The RectangleIntersectionBuilder is used to maintain lists of polygons,
 * linestrings and points left from clipping a {@link Geometry} with a {@link Rectangle}.
 * Once all clipping has been done, the class builds a valid {@link Geometry} from
 * the components.
 *
 * This is a utility class needed by {@link RectangleIntersection}, and is not
 * intended for public use.
 */

class GEOS_DLL RectangleIntersectionBuilder
{
  // Regular users are not supposed to use this utility class.
  friend class RectangleIntersection;

public:

  ~RectangleIntersectionBuilder();

private:

  // Building the final results
  geom::Geometry * build();
  geom::Geometry * internalBuild() const;

  // Utility methods needed for that, also outside this class
  void reconnectPolygons(const Rectangle & rect);
  void reconnect();
  void release(RectangleIntersectionBuilder & parts);

  // Adding Geometry components
  void add(geom::Polygon * g);
  void add(geom::LineString * g);
  void add(geom::Point * g);

  // Trivial methods
  bool empty() const;
  void clear();

  // Added components
  std::list<geom::Polygon *> polygons;
  std::list<geom::LineString *> lines;
  std::list<geom::Point *> points;

}; // class RectangleIntersectionBuilder

} // namespace geos::operation::intersection
} // namespace geos::operation
} // namespace geos

#endif // GEOS_OP_INTERSECTION_RECTANGLEINTERSECTIONBUILDER_H
