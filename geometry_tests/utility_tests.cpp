#include <dak/geometry/utility.h>
#include <dak/geometry/point.h>
#include <dak/geometry/constants.h>

#include <cmath>

#include "test_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{
   TEST_CLASS(utility_tests)
   {
   public:

      TEST_METHOD(utility_near)
      {
         Assert::IsTrue(near_zero(0., 0.));
         Assert::IsTrue(near_zero(0., 0.1));
         Assert::IsTrue(near_zero(0.1, 0.1));
         Assert::IsTrue(near_zero(0.));

         Assert::IsTrue(near_zero(TOLERANCE, TOLERANCE));
         Assert::IsTrue(near_zero(TOLERANCE/2, TOLERANCE));
         Assert::IsTrue(near_zero(TOLERANCE, 0.1));
         Assert::IsTrue(near_zero(TOLERANCE));
         Assert::IsTrue(near_zero(TOLERANCE/2));

         Assert::IsFalse(near_zero(TOLERANCE, 0.));
         Assert::IsFalse(near_zero(TOLERANCE/2, 0.));
         Assert::IsFalse(near_zero(TOLERANCE * 2, TOLERANCE));
         Assert::IsFalse(near_zero(0.1 + 0.00000001, 0.1));
         Assert::IsFalse(near_zero(TOLERANCE * 2));

         Assert::IsTrue(near_zero(point::origin()));
         Assert::IsTrue(near_zero(point(0., 0.)));
         Assert::IsTrue(near_zero(point(TOLERANCE, 0.)));
         Assert::IsTrue(near_zero(point(0., TOLERANCE)));
         Assert::IsTrue(near_zero(point(TOLERANCE/2, TOLERANCE/2)));

         Assert::IsFalse(near_zero(point(TOLERANCE * TOLERANCE + TOLERANCE, 0.)));
         Assert::IsFalse(near_zero(point(0., TOLERANCE * TOLERANCE + TOLERANCE)));
         Assert::IsFalse(near_zero(point(TOLERANCE * TOLERANCE + TOLERANCE, 0.)));
         Assert::IsFalse(near_zero(point(TOLERANCE * 1.1, 0.)));
         Assert::IsFalse(near_zero(point(0., TOLERANCE * 1.1)));
         Assert::IsFalse(near_zero(point(TOLERANCE, TOLERANCE)));
         Assert::IsFalse(near_zero(point(TOLERANCE, TOLERANCE * 2)));
         Assert::IsFalse(near_zero(point(TOLERANCE * 2, TOLERANCE * 2)));

         Assert::IsTrue(near(0., 0.));
         Assert::IsTrue(near(0., TOLERANCE));
         Assert::IsTrue(near(0., TOLERANCE/2));
         Assert::IsTrue(near(0.25, 0.25 + TOLERANCE - DBL_EPSILON));
         Assert::IsTrue(near(0.25, 0.25 + TOLERANCE/2));

         Assert::IsFalse(near(0.,    0. + TOLERANCE * 2));
         Assert::IsFalse(near(0.25,  0.25 + TOLERANCE * 2));
         Assert::IsFalse(near(0.25,  0.26));
         Assert::IsFalse(near(0.25, -3.26));
         Assert::IsFalse(near(0.25, -0.25));

         Assert::IsTrue(near(point::origin(), point::origin()));
         Assert::IsTrue(near(point(0., 0.), point(0., 0.)));
         Assert::IsTrue(near(point(1., 1.), point(1., 1.)));
         Assert::IsTrue(near(point(0., 0.), point(TOLERANCE, 0.)));
         Assert::IsTrue(near(point(0., 0.), point(0., TOLERANCE)));
         Assert::IsTrue(near(point(0., 0.), point(TOLERANCE/2, TOLERANCE/2)));

         Assert::IsFalse(near(point(0., 0.), point(TOLERANCE * 1.1, 0.)));
         Assert::IsFalse(near(point(0., 0.), point(0., TOLERANCE * 1.1)));
         Assert::IsFalse(near(point(0., 0.), point(TOLERANCE * 2, TOLERANCE)));
         Assert::IsFalse(near(point(0., 0.), point(TOLERANCE, TOLERANCE * 2)));
         Assert::IsFalse(near(point(0., 0.), point(TOLERANCE * 2, TOLERANCE * 2)));
         Assert::IsFalse(near(point(0., 0.), point(TOLERANCE * TOLERANCE + TOLERANCE, 0.)));
         Assert::IsFalse(near(point(0., 0.), point(0., TOLERANCE * TOLERANCE + TOLERANCE)));

         const double tolerance = 0.1;
         Assert::IsTrue(near(point::origin(), point::origin(), tolerance * tolerance));
         Assert::IsTrue(near(point(0., 0.), point(0., 0.), tolerance * tolerance));
         Assert::IsTrue(near(point(1., 1.), point(1., 1.), tolerance * tolerance));
         Assert::IsTrue(near(point(0., 0.), point(tolerance, 0.), tolerance * tolerance));
         Assert::IsTrue(near(point(0., 0.), point(0., tolerance), tolerance * tolerance));
         Assert::IsTrue(near(point(0., 0.), point(tolerance/2, tolerance/2), tolerance * tolerance));

         Assert::IsFalse(near(point(0., 0.), point(tolerance * 2, 0.), tolerance * tolerance));
         Assert::IsFalse(near(point(0., 0.), point(0., tolerance * 2), tolerance * tolerance));
         Assert::IsFalse(near(point(0., 0.), point(tolerance, tolerance), tolerance * tolerance));
         Assert::IsFalse(near(point(0., 0.), point(tolerance * 2, tolerance), tolerance * tolerance));
         Assert::IsFalse(near(point(0., 0.), point(tolerance, tolerance * 2), tolerance * tolerance));
         Assert::IsFalse(near(point(0., 0.), point(tolerance * 2, tolerance * 2), tolerance * tolerance));
         Assert::IsFalse(near(point(0., 0.), point(tolerance * tolerance + tolerance, 0.), tolerance * tolerance));
         Assert::IsFalse(near(point(0., 0.), point(0., tolerance * tolerance + tolerance), tolerance * tolerance));

         Assert::IsTrue(near_less( 0.,  0.1));
         Assert::IsTrue(near_less(-0.1, 0.));
         Assert::IsTrue(near_less(0., 0. + TOLERANCE * 2));

         Assert::IsFalse(near_less(0., 0.));
         Assert::IsFalse(near_less(0., 0. + TOLERANCE));
         Assert::IsFalse(near_less(0., 0. + TOLERANCE/2));

         Assert::IsTrue(near_greater(0.1, 0.));
         Assert::IsTrue(near_greater(0., -0.1));
         Assert::IsTrue(near_greater(0. + TOLERANCE * 2., 0.));

         Assert::IsFalse(near_greater(0., 0.));
         Assert::IsFalse(near_greater(0. + TOLERANCE, 0.));
         Assert::IsFalse(near_greater(0. + TOLERANCE / 2, 0.));
      }

      TEST_METHOD(utility_center)
      {
         const std::vector<point> pts({ point(1., 1.), point(-1., 1.), point(-1., -1.), point(1., -1.) });

         Assert::AreEqual(point(0., 0.), center(pts));

         const std::vector<point> new_pts = translate(pts, point(-3., 5.));

         Assert::AreEqual(point(-3., 5.), center(new_pts));
      }

      TEST_METHOD(utility_arc)
      {
         Assert::AreEqual(point( 1.,  0.), get_arc(0));
         Assert::AreEqual(point( 0.,  1.), get_arc(0.25));
         Assert::AreEqual(point(-1.,  0.), get_arc(0.5));
         Assert::AreEqual(point( 0., -1.), get_arc(0.75));
      }

      TEST_METHOD(utility_colinear)
      {
         Assert::IsTrue(is_colinear(point(0., 0.), point(0., 1.), point(0., 2.)));
         Assert::IsTrue(is_colinear(point(10., 0.), point(10., 1.), point(10., 2.)));
         Assert::IsTrue(is_colinear(point(0., 0.), point(1., 0.), point(2., 0.)));
         Assert::IsTrue(is_colinear(point(10., 0.), point(11., 0.), point(12., 0.)));
         Assert::IsTrue(is_colinear(point(10., 10.), point(11., 10.), point(12., 10.)));
         Assert::IsTrue(is_colinear(point(10., 10.), point(0., 0.), point(-12., -12.)));
         Assert::IsTrue(is_colinear(point(2., 10.), point(0., 0.), point(-2., -10.)));
         Assert::IsTrue(is_colinear(point(1., 2.), point(3., 4.), point(5., 6.)));

         Assert::IsFalse(is_colinear(point(1., 2.), point(3., 4.), point(5., 7.)));
         Assert::IsFalse(is_colinear(point(2., 2.), point(0., 0.), point(-2., 0.)));
         Assert::IsFalse(is_colinear(point(1., 2.), point(2., 3.), point(3., 2.)));
         Assert::IsFalse(is_colinear(point(1., 2.), point(2., 1.), point(3., 2.)));
      }
   };
}