#include <dak/geometry/point.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/constants.h>

#include <cmath>

#include "test_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{		
	TEST_CLASS(point_tests)
	{
	public:
		
		TEST_METHOD(point_constructors_and_comparisons)
		{
         const point p1;

         Assert::IsTrue(p1.is_invalid());
         Assert::AreEqual(point::invalid, p1);
         Assert::IsTrue(point::invalid == p1);
         Assert::IsTrue(p1 == p1);
         Assert::IsFalse(p1 < p1);

         const point p2(1., 2.);
         Assert::AreEqual(1., p2.x);
         Assert::AreEqual(2., p2.y);

         Assert::AreNotEqual(p1, p2);
         Assert::AreEqual(p2, p2);
         Assert::AreNotEqual(point::invalid, p2);
         Assert::IsFalse(p2.is_invalid());
         Assert::IsTrue(p1 != p2);
         Assert::IsTrue(p2 == p2);
         Assert::IsTrue(point::invalid != p2);
         Assert::IsFalse(p1 < p2);
         Assert::IsFalse(p2 < p2);
         Assert::IsFalse(point::invalid < p2);

         const point p3(2., 1.);
         Assert::AreEqual(2., p3.x);
         Assert::AreEqual(1., p3.y);

         Assert::AreNotEqual(p1, p3);
         Assert::AreNotEqual(p2, p3);
         Assert::AreEqual(p3, p3);
         Assert::AreNotEqual(point::invalid, p3);
         Assert::IsFalse(p3.is_invalid());
         Assert::IsTrue(p1 != p3);
         Assert::IsTrue(p2 != p3);
         Assert::IsTrue(p3 == p3);
         Assert::IsTrue(point::invalid != p3);
         Assert::IsFalse(p1 < p3);
         Assert::IsTrue(p2 < p3);
         Assert::IsFalse(p3 < p3);
         Assert::IsFalse(point::invalid < p3);

         const point p4(p3);
         Assert::AreEqual(2., p4.x);
         Assert::AreEqual(1., p4.y);

         Assert::AreNotEqual(p1, p4);
         Assert::AreNotEqual(p2, p4);
         Assert::AreEqual(p3, p4);
         Assert::AreEqual(p4, p4);
         Assert::AreNotEqual(point::invalid, p4);
         Assert::IsFalse(p4.is_invalid());
         Assert::IsTrue(p1 != p4);
         Assert::IsTrue(p2 != p4);
         Assert::IsTrue(p3 == p4);
         Assert::IsTrue(p4 == p4);
         Assert::IsTrue(point::invalid != p4);
         Assert::IsFalse(p1 < p4);
         Assert::IsTrue(p2 < p4);
         Assert::IsFalse(p3 < p4);
         Assert::IsFalse(p4 < p4);
         Assert::IsFalse(point::invalid < p4);

         Assert::AreEqual(1., point::unit_x().x);
         Assert::AreEqual(0., point::unit_x().y);

         Assert::AreEqual(0., point::unit_y().x);
         Assert::AreEqual(1., point::unit_y().y);

         Assert::AreEqual(0., point::origin().y);
         Assert::AreEqual(0., point::origin().y);

         point p5(1., 2.);
         point p6(3., 4.);
         Assert::AreEqual(1., p5.x);
         Assert::AreEqual(2., p5.y);
         Assert::AreEqual(3., p6.x);
         Assert::AreEqual(4., p6.y);

         p5.swap(p6);
         Assert::AreEqual(3., p5.x);
         Assert::AreEqual(4., p5.y);
         Assert::AreEqual(1., p6.x);
         Assert::AreEqual(2., p6.y);
      }

		TEST_METHOD(point_magnitude_and_distance)
		{
         const point p1(1., 2.);
         const point p2(-7., -5.);

         Assert::AreEqual(5., p1.mag_2());
         Assert::AreEqual(74., p2.mag_2());

         Assert::AreEqual(std::sqrt(5.), p1.mag(), 0.00001);
         Assert::AreEqual(std::sqrt(74.), p2.mag(), 0.00001);

         Assert::AreEqual(113., p1.distance_2(p2));
         Assert::AreEqual(std::sqrt(113.), p1.distance(p2), 0.00001);

         point p3(-7., 5.);

         Assert::AreEqual(64., p1.distance_2_to_line(p2, p3));
         Assert::AreEqual(8., p1.distance_to_line(p2, p3));
      }

		TEST_METHOD(point_calculations)
		{
         const point p1(-1., 2.);
         const point p2(3., -7.);

         Assert::AreEqual(1., p1.normalize().mag(), 0.00001);
         Assert::AreEqual(1., p2.normalize().mag(), 0.00001);

         {
            const point sum = p1 + p2;
            Assert::AreEqual(2., sum.x);
            Assert::AreEqual(-5., sum.y);
            Assert::AreEqual(point(2., -5.), sum);
         }

         {
            const point diff = p1 - p2;
            Assert::AreEqual(-4., diff.x);
            Assert::AreEqual(9., diff.y);
            Assert::AreEqual(point(-4., 9.), diff);
         }

         {
            const double dot = p1.dot(p2);
            Assert::AreEqual(-17., dot);
         }

         {
            const double dot = p2.dot(p1);
            Assert::AreEqual(-17., dot);
         }

         {
            const point scale = p1.scale(4.);
            Assert::AreEqual(-4., scale.x);
            Assert::AreEqual(8., scale.y);
            Assert::AreEqual(point(-4., 8.), scale);
         }

         {
            const point scale = p2.scale(4., -3.);
            Assert::AreEqual(12., scale.x);
            Assert::AreEqual(21., scale.y);
            Assert::AreEqual(point(12., 21.), scale);
         }

         {
            const point perp = p2.perp();
            Assert::AreEqual(7., perp.x);
            Assert::AreEqual(3., perp.y);
            Assert::AreEqual(point(7., 3.), perp);
            Assert::AreEqual(-58., perp.cross(p2));
            Assert::AreEqual(-1., perp.normalize().cross(p2.normalize()), 0.00001);
         }

         Assert::AreEqual(0., point::unit_x().cross(point::unit_x()));
         Assert::AreEqual(0., point::unit_y().cross(point::unit_y()));
         Assert::AreEqual(1., point::unit_x().cross(point::unit_y()));
         Assert::AreEqual(-1., point::unit_y().cross(point::unit_x()));

         {
            point convex_sum = p1.convex_sum(p2, 0.5);
            Assert::AreEqual(1., convex_sum.x);
            Assert::AreEqual(-2.5, convex_sum.y);
            Assert::AreEqual(point(1., -2.5), convex_sum);
         }
      }

      TEST_METHOD(point_angles)
      {
         Assert::AreEqual( 3 * PI / 4, point::unit_x().angle(point::unit_y()));
         Assert::AreEqual(-1 * PI / 4, point::unit_y().angle(point::unit_x()));
         Assert::AreEqual(0., point::unit_x().angle());
         Assert::AreEqual(PI / 2, point::unit_y().angle());

         Assert::AreEqual(PI / 2, point::origin().sweep(point::unit_x(), point::unit_y()));
         Assert::AreEqual(3 * PI / 2, point::origin().sweep(point::unit_y(), point::unit_x()));
      }

      TEST_METHOD(point_transform)
      {
         {
            const point p1(1., 2.);

            Assert::AreEqual(point(-2., 1.), p1.apply(transform::rotate(PI / 2)));
            Assert::AreEqual(point(-1., -2.), p1.apply(transform::rotate(PI)));
            Assert::AreEqual(point(2., -1.), p1.apply(transform::rotate(3 * PI / 2)));
            Assert::AreEqual(point(1., 2.), p1.apply(transform::rotate(2 * PI)));
         }

         {
            Assert::AreEqual(point(0., 1.), point::unit_x().apply(transform::rotate(PI / 2)));
            Assert::AreEqual(point(-1., 0.), point::unit_x().apply(transform::rotate(PI)));
            Assert::AreEqual(point(0., -1.), point::unit_x().apply(transform::rotate(3 * PI / 2)));
            Assert::AreEqual(point(1., 0.), point::unit_x().apply(transform::rotate(2 * PI)));

            Assert::AreEqual(point(std::sqrt(2.) / 2., std::sqrt(2.) / 2.), point::unit_x().apply(transform::rotate(PI / 4)));
         }

         {
            const point p1(1., 2.);

            Assert::AreEqual(point(3., 5.), p1.apply(transform::translate(2., 3.)));
            Assert::AreEqual(point(-2., -5.), p1.apply(transform::translate(point(-3., -7.))));
         }

         {
            const point p1(1., 2.);

            Assert::AreEqual(point(2., 4.), p1.apply(transform::scale(2.)));
            Assert::AreEqual(point(-3., -14.), p1.apply(transform::scale(-3., -7.)));
         }

      }

      TEST_METHOD(point_change_basis)
      {
         transform basis_change = transform::to_basis(point(3., 1.), point(-2., 1.));

         const point p1(4., 3.);

         const point p1_in_uv = p1.apply(basis_change);
         Assert::AreEqual(point(2., 1.), p1_in_uv);
      }
   };
}