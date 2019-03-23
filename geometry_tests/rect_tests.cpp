#include <dak/geometry/rect.h>
#include <dak/geometry/transform.h>

#include <cmath>

#include "test_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{
   TEST_CLASS(rect_tests)
   {
   public:

      TEST_METHOD(rect_constructor)
      {
         rect r1;

         Assert::IsTrue(r1.is_invalid());
         Assert::AreEqual(point(), r1.center());
         Assert::AreEqual(rect(), r1);
         Assert::IsTrue(rect() == r1);
         Assert::IsFalse(rect() != r1);
         Assert::IsTrue(r1 == r1);
         Assert::IsFalse(r1 != r1);

         rect r2(0, 0, 1, 1);

         Assert::IsFalse(r2.is_invalid());
         Assert::AreEqual(point(0.5, 0.5), r2.center());
         Assert::AreEqual(rect(0, 0, 1, 1), r2);
         Assert::IsTrue(rect(0, 0, 1, 1) == r2);
         Assert::IsFalse(rect(0, 0, 1, 1) != r2);
         Assert::IsTrue(r2 == r2);
         Assert::IsFalse(r2 != r2);

         Assert::IsFalse(rect() == r2);
         Assert::IsFalse(r1 == r2);
         Assert::IsTrue(rect() != r2);
         Assert::IsTrue(r1 != r2);
         Assert::AreNotEqual(r1, r2);

         rect r3(point(4, 5), 6, 7);

         Assert::IsFalse(r3.is_invalid());
         Assert::AreEqual(point(7., 8.5), r3.center());
         Assert::AreEqual(rect(point(4, 5), 6, 7), r3);
         Assert::IsTrue(rect(point(4, 5), 6, 7) == r3);
         Assert::IsFalse(rect(point(4, 5), 6, 7) != r3);
         Assert::AreEqual(rect(4, 5, 6, 7), r3);
         Assert::IsTrue(rect(4, 5, 6, 7) == r3);
         Assert::IsFalse(rect(4, 5, 6, 7) != r3);
         Assert::IsTrue(r3 == r3);
         Assert::IsFalse(r3 != r3);

         Assert::IsFalse(rect() == r3);
         Assert::IsFalse(r1 == r3);
         Assert::IsTrue(rect() != r3);
         Assert::IsTrue(r1 != r3);
         Assert::AreNotEqual(r1, r3);

         Assert::IsFalse(rect(0, 0, 1, 1) == r3);
         Assert::IsFalse(r2 == r3);
         Assert::IsTrue(rect(0, 0, 1, 1) != r3);
         Assert::IsTrue(r2 != r3);
         Assert::AreNotEqual(r2, r3);

         rect r4(point(4, 5), point(6, 7));

         Assert::IsFalse(r4.is_invalid());
         Assert::AreEqual(point(5., 6.), r4.center());
         Assert::AreEqual(rect(point(4, 5), point(6, 7)), r4);
         Assert::IsTrue(rect(point(4, 5), point(6, 7)) == r4);
         Assert::IsFalse(rect(point(4, 5), point(6, 7)) != r4);
         Assert::AreEqual(rect(4, 5, 2, 2), r4);
         Assert::IsTrue(rect(4, 5, 2, 2) == r4);
         Assert::IsFalse(rect(4, 5, 2, 2) != r4);
         Assert::IsTrue(r4 == r4);
         Assert::IsFalse(r4 != r4);

         Assert::IsFalse(rect() == r4);
         Assert::IsFalse(r1 == r4);
         Assert::IsTrue(rect() != r4);
         Assert::IsTrue(r1 != r4);
         Assert::AreNotEqual(r1, r4);

         Assert::IsFalse(rect(0, 0, 1, 1) == r4);
         Assert::IsFalse(r2 == r4);
         Assert::IsTrue(rect(0, 0, 1, 1) != r4);
         Assert::IsTrue(r2 != r4);
         Assert::AreNotEqual(r2, r4);

         Assert::IsFalse(rect(point(4, 5), 6, 7) == r4);
         Assert::IsFalse(r3 == r4);
         Assert::IsTrue(rect(point(4, 5), 6, 7) != r4);
         Assert::IsTrue(r3 != r4);
         Assert::AreNotEqual(r3, r4);
      }

      TEST_METHOD(rect_calculations)
      {
         rect r1(0, 0, 10, 10);
         rect r2(4, -4, 20, 20);

         Assert::AreEqual(rect(0, -4, 24, 20), r1.combine(r2));
         Assert::AreEqual(rect(4, 0, 6, 10), r1.intersect(r2));

         rect r3(-14, -14, 10, 10);
         Assert::AreEqual(rect(), r1.intersect(r3));
         Assert::IsTrue(r1.intersect(r3).is_invalid());

         Assert::AreEqual(rect(-11.5, -11.5, 5, 5), r3.central_scale(0.5));
         Assert::AreEqual(rect(-10, -10, 2, 2), r3.central_scale(0.2));
      }

      TEST_METHOD(rect_center_inside)
      {
         rect small(0, 0, 10, 10);
         rect big(-5, -5, 20, 20);

         Assert::AreEqual(transform::translate(2.5, 2.5).compose(transform::scale(0.5)), big.center_inside(small));

         Assert::AreEqual(transform::translate(-5, -5).compose(transform::scale(2)), small.center_inside(big));
      }
   };
}