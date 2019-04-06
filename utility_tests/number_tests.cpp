#include <dak/utility/number.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::utility;

namespace geometry_tests
{
   TEST_CLASS(utility_number_tests)
   {
   public:

      TEST_METHOD(utility_near)
      {
         Assert::IsTrue(near_zero(0., 0.));
         Assert::IsTrue(near_zero(0., 0.1));
         Assert::IsTrue(near_zero(0.1, 0.1));
         Assert::IsTrue(near_zero(0.));

         Assert::IsTrue(near_zero(dak::utility::TOLERANCE, dak::utility::TOLERANCE));
         Assert::IsTrue(near_zero(dak::utility::TOLERANCE/2, dak::utility::TOLERANCE));
         Assert::IsTrue(near_zero(dak::utility::TOLERANCE, 0.1));
         Assert::IsTrue(near_zero(dak::utility::TOLERANCE));
         Assert::IsTrue(near_zero(dak::utility::TOLERANCE/2));

         Assert::IsFalse(near_zero(dak::utility::TOLERANCE, 0.));
         Assert::IsFalse(near_zero(dak::utility::TOLERANCE/2, 0.));
         Assert::IsFalse(near_zero(dak::utility::TOLERANCE * 2, dak::utility::TOLERANCE));
         Assert::IsFalse(near_zero(0.1 + 0.00000001, 0.1));
         Assert::IsFalse(near_zero(dak::utility::TOLERANCE * 2));

         Assert::IsTrue(near(0., 0.));
         Assert::IsTrue(near(0., dak::utility::TOLERANCE));
         Assert::IsTrue(near(0., dak::utility::TOLERANCE/2));
         Assert::IsTrue(near(0.25, 0.25 + dak::utility::TOLERANCE - DBL_EPSILON));
         Assert::IsTrue(near(0.25, 0.25 + dak::utility::TOLERANCE/2));

         Assert::IsFalse(near(0.,    0. + dak::utility::TOLERANCE * 2));
         Assert::IsFalse(near(0.25,  0.25 + dak::utility::TOLERANCE * 2));
         Assert::IsFalse(near(0.25,  0.26));
         Assert::IsFalse(near(0.25, -3.26));
         Assert::IsFalse(near(0.25, -0.25));

         Assert::IsTrue(near_less( 0.,  0.1));
         Assert::IsTrue(near_less(-0.1, 0.));
         Assert::IsTrue(near_less(0., 0. + dak::utility::TOLERANCE * 2));

         Assert::IsFalse(near_less(0., 0.));
         Assert::IsFalse(near_less(0., 0. + dak::utility::TOLERANCE));
         Assert::IsFalse(near_less(0., 0. + dak::utility::TOLERANCE/2));

         Assert::IsTrue(near_less_or_equal( 0.,  0.1));
         Assert::IsTrue(near_less_or_equal(-0.1, 0.));
         Assert::IsTrue(near_less_or_equal(0., 0. + dak::utility::TOLERANCE * 2));

         Assert::IsTrue(near_less_or_equal(0., 0.));
         Assert::IsTrue(near_less_or_equal(0., 0. + dak::utility::TOLERANCE));
         Assert::IsTrue(near_less_or_equal(0., 0. + dak::utility::TOLERANCE/2));

         Assert::IsTrue(near_greater_or_equal(0.1, 0.));
         Assert::IsTrue(near_greater_or_equal(0., -0.1));
         Assert::IsTrue(near_greater_or_equal(0. + dak::utility::TOLERANCE * 2., 0.));

         Assert::IsTrue(near_greater_or_equal(0., 0.));
         Assert::IsTrue(near_greater_or_equal(0. + dak::utility::TOLERANCE, 0.));
         Assert::IsTrue(near_greater_or_equal(0. + dak::utility::TOLERANCE / 2, 0.));
      }
   };
}