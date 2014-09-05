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
#include <geos/util/IllegalArgumentException.h>

namespace geos {
namespace operation { // geos::operation
namespace intersection { // geos::operation::intersection

  /*
   * Create a clipping rectangle
   */

  Rectangle(double x1, double y1, double x2, double y2)
	: xMin(x1)
	, yMin(y1)
	, xMax(x2)
	, yMax(y2)
  {
	if(xMin >= xMax || yMin >= yMax)
	  {
		throw util::IllegalArgumentException("Clipping rectangle must be non-empty")
	  }
  }

} // namespace geos::operation::intersection
} // namespace geos::operation
} // namespace geos
