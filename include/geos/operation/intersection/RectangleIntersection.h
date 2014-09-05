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

#ifndef GEOS_OP_RECTANGLE_INTERSECTION_H
#define GEOS_OP_RECTANGLE_INTERSECTION_H

#include <geos/export.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4251) // warning C4251: needs to have dll-interface to be used by clients of class
#endif

// Forward declarations
namespace geos {
  namespace geom {
	class Geometry;
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
 * \brief Speed-optimized clipping of a {@link Geometry} with a rectangle.
 *
 * Two different methods are provided. The first performs normal
 * clipping, the second clips the boundaries of polygons, not
 * the polygons themselves. In the first case a polygon will remain
 * a polygon or is completely cut out. In the latter case polygons
 * will be converted to polylines if any vertex is outside the clipping
 * rectangle, or will be cut out completely.
 *
 * The algorithm works best when the number of intersections is very low.
 * For example, if the geometry is completely to the left of the
 * clipping rectangle, only the x-coordinate of the geometry is ever
 * tested and is only compared with the x-coordinate of the left edge
 * of the rectangle. Hence clipping may be faster than calculating
 * the envelope of the geometry for trivial overlap tests.
 *
 * The input geometry must be valid. In particular all {@link LinearRing}s must
 * be properly closed, or the algorithm may not terminate.
 *
 */

class GEOS_DLL RectangleIntersection
{
 public:

  /**
   * \brief Clip geometry with a rectangle
   *
   * @param geom a {@link Geometry}
   * @param rect a {@link Rectangle}
   * @return the clipped geometry
   * @return NULL if the geometry is outside the {@link Rectangle}
   */

  static geom::Geometry * clip(const geom::Geometry & geom,
							   const Rectangle & rect);

  /**
   * \brief Clip boundary of a geometry with a rectangle
   *
   * @param geom a {@link Geometry}
   * @param rect a {@link Rectangle}
   * @return the clipped geometry
   * @return NULL if the geometry is outside the {@link Rectangle}
   */

  static geom::Geometry * clipBoundary(const geom::Geometry & geom,
									   const Rectangle & rect);

}; // class RectangleIntersection

} // namespace geos::operation::intersection
} // namespace geos::operation
} // namespace geos

#endif // GEOS_OP_RECTANGLE_INTERSECTION_H
