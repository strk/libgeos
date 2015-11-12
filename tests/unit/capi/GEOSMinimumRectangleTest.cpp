//
// Test Suite for C-API GEOSMinimumRectangle

#include <tut.hpp>
// geos
#include <geos_c.h>
// std
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

namespace tut
{
    //
    // Test Group
    //

    // Common data used in test cases.
    struct test_capigeosminimumrectangle_data
    {
        GEOSGeometry* input_;
        GEOSGeometry* expected_;

        static void notice(const char *fmt, ...)
        {
            std::fprintf( stdout, "NOTICE: ");

            va_list ap;
            va_start(ap, fmt);
            std::vfprintf(stdout, fmt, ap);
            va_end(ap);

            std::fprintf(stdout, "\n");
        }

        test_capigeosminimumrectangle_data()
            : input_(0), expected_(0)
        {
            initGEOS(notice, notice);
        }

        ~test_capigeosminimumrectangle_data()
        {
            GEOSGeom_destroy(input_);
            GEOSGeom_destroy(expected_);
            input_ = 0;
            expected_ = 0;
            finishGEOS();
        }

    };

    typedef test_group<test_capigeosminimumrectangle_data> group;
    typedef group::object object;

    group test_capigeosminimumrectangle_group("capi::GEOSMinimumRectangle");

    //
    // Test Cases
    //

    template<>
    template<>
    void object::test<1>()
    {
        input_ = GEOSGeomFromWKT("POLYGON ((0 5, 5 10, 10 5, 5 0, 0 5))");
        ensure( 0 != input_ );

        expected_ = GEOSGeomFromWKT("POLYGON ((5 0, 10 5, 5 10, 0 5, 5 0))");
        ensure( 0 != expected_ );

        GEOSGeometry* output = GEOSMinimumRectangle(input_);
        ensure( 0 != output );
        ensure( 0 == GEOSisEmpty(output) );
        // TODO
        //ensure( 0 != GEOSEquals(output, expected_));
        GEOSGeom_destroy(output);
    }

} // namespace tut
