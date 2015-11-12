/**********************************************************************
 *
 * GEOS - Geometry Engine Open Source
 * http://geos.osgeo.org
 *
 * Copyright (C) 2015      Nyall Dawson <nyall dot dawson at gmail dot com>
 *
 * This is free software; you can redistribute and/or modify it under
 * the terms of the GNU Lesser General Public Licence as published
 * by the Free Software Foundation.
 * See the COPYING file for more information.
 *
 **********************************************************************/

//
// Test Suite for geos::algorithm::MinimumDiameter

#include <tut.hpp>
// geos
#include <geos/geom/Coordinate.h>
#include <geos/algorithm/MinimumDiameter.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>
#include <geos/geom/Geometry.h>
// std
#include <sstream>
#include <string>
#include <memory>

namespace tut
{
    //
    // Test Group
    //

    // dummy data, not used
    struct test_minimumdiameter_data {
      typedef geos::geom::Geometry Geometry;
      typedef std::auto_ptr<geos::geom::Geometry> GeomPtr;

      typedef geos::geom::Coordinate Coordinate;
      typedef geos::algorithm::MinimumDiameter MinimumDiameter;

      geos::io::WKTReader reader;
      std::auto_ptr<Geometry> geom;

      test_minimumdiameter_data()
      {}

    };

    typedef test_group<test_minimumdiameter_data> group;
    typedef group::object object;

    group test_minimumdiameter_data_group("geos::algorithm::MinimumDiameter");

    //
    // Test Cases
    //

    // Test of getMinimumRectangle
    template<>
    template<>
    void object::test<1>()
    {
        GeomPtr geom(reader.read("POLYGON ((0 0, 0 20, 20 20, 20 0, 0 0))"));
        ensure(0 != geom.get());

        geos::algorithm::MinimumDiameter m(geom.get());
        GeomPtr minRect( m.getMinimumRectangle() );
        ensure(0 != minRect.get());

        GeomPtr expectedGeom(reader.read("POLYGON ((0 0, 20 0, 20 20, 0 20, 0 0))"));
        ensure(0 != expectedGeom.get());

        ensure( minRect.get()->equalsExact(expectedGeom.get()) );
    }

    // Test with expected rotated rectangle
    template<>
    template<>
    void object::test<2>()
    {
        GeomPtr geom(reader.read("POLYGON ((0 5, 5 10, 10 5, 5 0, 0 5))"));
        ensure(0 != geom.get());

        geos::algorithm::MinimumDiameter m(geom.get());
        GeomPtr minRect( m.getMinimumRectangle() );
        ensure(0 != minRect.get());

        GeomPtr expectedGeom(reader.read("POLYGON ((5 0, 10 5, 5 10, 0 5, 5 0))"));
        ensure(0 != expectedGeom.get());

        ensure( minRect.get()->equalsExact(expectedGeom.get()) );
    }

} // namespace tut
