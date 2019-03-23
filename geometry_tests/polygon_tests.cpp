#include <dak/geometry/polygon.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/constants.h>

#include <cmath>

#include "CppUnitTest.h"

#include "test_helpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{
   TEST_CLASS(polygon_tests)
   {
   public:

      TEST_METHOD(polygon_constructors_and_comparisons)
      {
         const polygon p1;

         Assert::IsTrue(p1.is_invalid());
         Assert::IsTrue(polygon() == p1);
         Assert::IsTrue(p1.center().is_invalid());

         const polygon p2({ point(1., 2.), point(-3., -4.), point(5, -6) });
         Assert::AreEqual( 1., p2.points[0].x);
         Assert::AreEqual( 2., p2.points[0].y);
         Assert::AreEqual(-3., p2.points[1].x);
         Assert::AreEqual(-4., p2.points[1].y);
         Assert::AreEqual( 5., p2.points[2].x);
         Assert::AreEqual(-6., p2.points[2].y);
         Assert::AreEqual(point(1., -8./3), p2.center());

         Assert::AreNotEqual(p1, p2);
         Assert::AreEqual(p2, p2);
         Assert::AreNotEqual(polygon(), p2);
         Assert::IsFalse(p2.is_invalid());
         Assert::IsTrue(p1 != p2);
         Assert::IsTrue(polygon() != p2);

         const polygon p3({ point(2., 1.), point(5., 7.), point(3., 6.) });
         Assert::AreEqual(2., p3.points[0].x);
         Assert::AreEqual(1., p3.points[0].y);
         Assert::AreEqual(5., p3.points[1].x);
         Assert::AreEqual(7., p3.points[1].y);
         Assert::AreEqual(3., p3.points[2].x);
         Assert::AreEqual(6., p3.points[2].y);
         Assert::AreEqual(point(10./3, 14. / 3), p3.center());

         Assert::AreNotEqual(p1, p3);
         Assert::AreNotEqual(p2, p3);
         Assert::AreEqual(p3, p3);
         Assert::AreNotEqual(polygon(), p3);
         Assert::IsFalse(p3.is_invalid());
         Assert::IsTrue(p1 != p3);
         Assert::IsTrue(p2 != p3);
         Assert::IsTrue(polygon() != p3);

         const polygon p4(p3);
         Assert::AreEqual(2., p4.points[0].x);
         Assert::AreEqual(1., p4.points[0].y);
         Assert::AreEqual(5., p4.points[1].x);
         Assert::AreEqual(7., p4.points[1].y);
         Assert::AreEqual(3., p4.points[2].x);
         Assert::AreEqual(6., p4.points[2].y);

         Assert::AreNotEqual(p1, p4);
         Assert::AreNotEqual(p2, p4);
         Assert::AreEqual(p3, p4);
         Assert::AreEqual(p4, p4);
         Assert::AreNotEqual(polygon(), p4);
         Assert::IsFalse(p4.is_invalid());
         Assert::IsTrue(p1 != p4);
         Assert::IsTrue(p2 != p4);
         Assert::IsTrue(p3 == p4);
         Assert::IsTrue(polygon() != p4);
      }

      TEST_METHOD(polygon_calculations)
      {
         polygon p1({ point(3., 3.), point(3., -3.), point(-3., -3.), point(-3., 3.), });

         Assert::AreEqual(36., p1.area());
         Assert::AreEqual(24., p1.perimeter());
         Assert::AreEqual(18., p1.perimeter(false));
         Assert::AreEqual(point(0, 0), p1.center());

         Assert::IsTrue(p1.is_inside(point::origin()));
         Assert::IsTrue(p1.is_inside(point::unit_x()));
         Assert::IsTrue(p1.is_inside(point::unit_y()));
         Assert::IsTrue(p1.is_inside(point(1., -1.)));

         Assert::IsFalse(p1.is_inside(point(1., -4.)));
         Assert::IsFalse(p1.is_inside(point(3., -4.)));
         Assert::IsFalse(p1.is_inside(point(4., 2.)));
         Assert::IsFalse(p1.is_inside(point(100., -400.)));
      }

      TEST_METHOD(polygon_regular)
      {
         // Two points or less is always true.
         {
            const polygon p1;
            Assert::IsTrue(p1.is_regular());
         }

         {
            const polygon p2({ point(1., 2.) });
            Assert::IsTrue(p2.is_regular());
            Assert::AreEqual(point(1., 2.), p2.center());
         }

         {
            const polygon p3({ point(-1., 2.), point(7., 2.) });
            Assert::IsTrue(p3.is_regular());
            Assert::AreEqual(point(3., 2), p3.center());
         }

         {
            // Four points forming a square around the origin is true.
            const polygon p4({ point( 1.,  1.), point(-1.,  1.),
                               point(-1., -1.), point( 1., -1.) });
            Assert::IsTrue(p4.is_regular());
            Assert::AreEqual(point(0., 0), p4.center());

            // Four points forming a square around the another point is true.
            const polygon p5( p4.apply( transform::translate( point(-4, 12) ) ) );
            Assert::AreEqual(point(-4., 12), p5.center());

            Assert::IsTrue(p5.is_regular());

            // Four points forming a square around the another point is true.
            const polygon p6( p4.apply( transform::rotate( point(-2, 3), PI / 5 ) ) );

            Assert::IsTrue(p6.is_regular());
         }

         {
            // Irregular triangle.
            polygon p7({ point(-1., 2.), point(7., 2.), point(4., 6.) });

            Assert::IsFalse(p7.is_regular());
         }

         {
            // Four points forming a losange around the origin is false.
            polygon p4({ point(1., 1.), point(-2., 1.), point(-1., -1.), point(2., -1.) });

            Assert::IsFalse(p4.is_regular());
         }

         // Regular polygon should be regular.
         for (int sides = 3; sides < 20; ++sides)
         {
            polygon rp = polygon::make_regular(sides);

            Assert::IsTrue(rp.is_regular());
            Assert::AreEqual(point(0., 0), rp.center());

            Assert::IsTrue(rp.apply(transform::translate(point(-4, 12))).is_regular());
            Assert::IsTrue(rp.apply(transform::rotate(point(-2, 3), PI / 5)).is_regular());
         }

         // Translated regular polygon should be regular.
         for (int sides = 3; sides < 20; ++sides)
         {
            const polygon rp(polygon::make_regular(sides).apply(transform::translate(point(-4, 12))));

            Assert::IsTrue(rp.is_regular());
            Assert::AreEqual(point(-4., 12), rp.center());
         }
      }

      TEST_METHOD(polygon_transform)
      {
         {
            const polygon p1({ point(1., 2.), point(-3., -4.) });

            Assert::AreEqual(polygon({ point(-2., 1.), point(4., -3.) }), p1.apply(transform::rotate(PI / 2)));
            Assert::AreEqual(polygon({ point(-1., -2.), point(3., 4.) }), p1.apply(transform::rotate(PI)));
            Assert::AreEqual(polygon({ point(2., -1.), point(-4., 3.) }), p1.apply(transform::rotate(3 * PI / 2)));
            Assert::AreEqual(polygon({ point(1., 2.), point(-3., -4.) }), p1.apply(transform::rotate(2 * PI)));
         }

         {
            Assert::AreEqual(polygon({ point::origin(), point(0., 1.) }), polygon({ point::origin(), point::unit_x() }).apply(transform::rotate(PI / 2)));
            Assert::AreEqual(polygon({ point::origin(), point(-1., 0.) }), polygon({ point::origin(), point::unit_x() }).apply(transform::rotate(PI)));
            Assert::AreEqual(polygon({ point::origin(), point(0., -1.) }), polygon({ point::origin(), point::unit_x() }).apply(transform::rotate(3 * PI / 2)));
            Assert::AreEqual(polygon({ point::origin(), point::unit_x() }), polygon({ point::origin(), point::unit_x() }).apply(transform::rotate(2 * PI)));

            Assert::AreEqual(polygon({ point::origin(), point(std::sqrt(2.) / 2., std::sqrt(2.) / 2.) }), polygon({ point::origin(), point::unit_x() }).apply(transform::rotate(PI / 4)));
         }

         {
            const polygon p1({ point(1., 2.), point(-3., -4.) });

            Assert::AreEqual(polygon({ point(3., 5.), point(-1., -1.) }), p1.apply(transform::translate(2., 3.)));
            Assert::AreEqual(polygon({ point(-2., -5.), point(-6., -11.) }), p1.apply(transform::translate(point(-3., -7.))));
         }

         {
            const polygon p1({ point(1., 2.), point(-3., -4.) });

            Assert::AreEqual(polygon({ point(2., 4.), point(-6., -8.) }), p1.apply(transform::scale(2.)));
            Assert::AreEqual(polygon({ point(-3., -14.), point(9., 28.) }), p1.apply(transform::scale(-3., -7.)));
         }
      }
   };
}