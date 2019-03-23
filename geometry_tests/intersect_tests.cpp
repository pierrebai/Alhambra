#include <dak/geometry/intersect.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/constants.h>

#include <cmath>

#include "test_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{
   TEST_CLASS(intersect_tests)
   {
   public:

      TEST_METHOD(stay_on_units_tests)
      {
         Assert::AreEqual(point::unit_x(), intersect::stay_on_units(point::unit_x()));
         Assert::AreEqual(point::unit_y(), intersect::stay_on_units(point::unit_y()));
         Assert::AreEqual(point::origin(), intersect::stay_on_units(point::origin()));

         Assert::AreEqual(point(1., 1.), intersect::stay_on_units(point(1., 1.)));

         Assert::AreEqual(point::invalid, intersect::stay_on_units(point(2., 1.)));
         Assert::AreEqual(point::invalid, intersect::stay_on_units(point(0., 2.)));
      }

      TEST_METHOD(stay_near_units_tests)
      {
         Assert::AreEqual(point::unit_x(), intersect::stay_near_units(point::unit_x()));
         Assert::AreEqual(point::unit_y(), intersect::stay_near_units(point::unit_y()));
         Assert::AreEqual(point::origin(), intersect::stay_near_units(point::origin()));

         Assert::AreEqual(point(1., 1.), intersect::stay_near_units(point(1., 1.)));

         Assert::AreEqual(point::invalid, intersect::stay_near_units(point(2., 1.)));
         Assert::AreEqual(point::invalid, intersect::stay_near_units(point(0., 2.)));

         Assert::AreEqual(point(1.2, 1.), intersect::stay_near_units(point(1.2, 1.)));
         Assert::AreEqual(point(1., 1.2), intersect::stay_near_units(point(1., 1.2)));

         Assert::AreEqual(point::invalid, intersect::stay_near_units(point(1., 1.21)));
         Assert::AreEqual(point::invalid, intersect::stay_near_units(point(1.21, 1.)));
      }

      TEST_METHOD(intersection_tests)
      {
         // Parallel
         Assert::AreEqual(point::invalid, intersect::intersect(point::origin(), point::unit_x(), point::origin(), point::unit_x()));
         Assert::AreEqual(point::invalid, intersect::intersect(point(0, 1), point(0, 4), point(3, 3), point(3, 7)));
         Assert::AreEqual(point::invalid, intersect::intersect(point(3, 1), point(9, 1), point(3, 3), point(9, 3)));

         // Barely touching.
         Assert::AreEqual(point::origin(), intersect::intersect(point::origin(), point::unit_x(), point::origin(), point::unit_y()));
         Assert::AreEqual(point(1, 1), intersect::intersect(point(1, 1), point(22, 32), point(67, -12), point(1, 1)));

         // Perpendicular, but not touching.
         Assert::AreEqual(point::invalid, intersect::intersect(point(3, 1), point(9, 1), point(5, 2), point(5, 6)));

         // Near miss.
         Assert::AreEqual(point::invalid, intersect::intersect(point(3, 1), point(9, 1), point(5, 1.0001), point(5, 2)));
         Assert::AreEqual(point::invalid, intersect::intersect(point(3, 2), point(9, 2), point(9.2, -2), point(9.2, 6)));

         // Crossing.
         Assert::AreEqual(point(5, 1), intersect::intersect(point(3, 1), point(9, 1), point(5, -2), point(5, 6)));
         Assert::AreEqual(point::origin(), intersect::intersect(point(1, 1), point(-1, -1), point(5, -5), point(-5, 5)));
      }

      TEST_METHOD(near_intersection_tests)
      {
         // Parallel
         Assert::AreEqual(point::invalid, intersect::intersect_near(point::origin(), point::unit_x(), point::origin(), point::unit_x()));
         Assert::AreEqual(point::invalid, intersect::intersect_near(point(0, 1), point(0, 4), point(3, 3), point(3, 7)));
         Assert::AreEqual(point::invalid, intersect::intersect_near(point(3, 1), point(9, 1), point(3, 3), point(9, 3)));

         // Barely touching.
         Assert::AreEqual(point::origin(), intersect::intersect_near(point::origin(), point::unit_x(), point::origin(), point::unit_y()));
         Assert::AreEqual(point(1, 1), intersect::intersect_near(point(1, 1), point(22, 32), point(67, -12), point(1, 1)));

         // Perpendicular, but not touching.
         Assert::AreEqual(point::invalid, intersect::intersect_near(point(3, 1), point(9, 1), point(5, 2), point(5, 6)));

         // Near miss.
         Assert::AreEqual(point(5, 1), intersect::intersect_near(point(3, 1), point(9, 1), point(5, 1.0001), point(5, 2)));
         Assert::AreEqual(point(9.2, 2), intersect::intersect_near(point(3, 2), point(9, 2), point(9.2, -2), point(9.2, 6)));

         // Crossing.
         Assert::AreEqual(point(5, 1), intersect::intersect_near(point(3, 1), point(9, 1), point(5, -2), point(5, 6)));
         Assert::AreEqual(point::origin(), intersect::intersect_near(point(1, 1), point(-1, -1), point(5, -5), point(-5, 5)));
      }

      TEST_METHOD(true_intersection_tests)
      {
         // Parallel
         Assert::AreEqual(point::invalid, intersect::intersect_within(point::origin(), point::unit_x(), point::origin(), point::unit_x()));
         Assert::AreEqual(point::invalid, intersect::intersect_within(point(0, 1), point(0, 4), point(3, 3), point(3, 7)));
         Assert::AreEqual(point::invalid, intersect::intersect_within(point(3, 1), point(9, 1), point(3, 3), point(9, 3)));

         // Barely touching.
         Assert::AreEqual(point::invalid, intersect::intersect_within(point::origin(), point::unit_x(), point::origin(), point::unit_y()));
         Assert::AreEqual(point::invalid, intersect::intersect_within(point(1, 1), point(22, 32), point(67, -12), point(1, 1)));

         // Perpendicular, but not touching.
         Assert::AreEqual(point::invalid, intersect::intersect_within(point(3, 1), point(9, 1), point(5, 2), point(5, 6)));

         // Near miss.
         Assert::AreEqual(point::invalid, intersect::intersect_within(point(3, 1), point(9, 1), point(5, 1.0001), point(5, 2)));
         Assert::AreEqual(point::invalid, intersect::intersect_within(point(3, 2), point(9, 2), point(9.2, -2), point(9.2, 6)));

         // Crossing.
         Assert::AreEqual(point(5, 1), intersect::intersect_within(point(3, 1), point(9, 1), point(5, -2), point(5, 6)));
         Assert::AreEqual(point::origin(), intersect::intersect_within(point(1, 1), point(-1, -1), point(5, -5), point(-5, 5)));
      }

   };
}