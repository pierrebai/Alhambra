#include <dak/geometry/edge.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/constants.h>

#include <cmath>

#include "test_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{		
	TEST_CLASS(edge_tests)
	{
	public:
		
		TEST_METHOD(edge_constructors_and_comparisons)
		{
         const edge e1;

         Assert::IsTrue(e1.is_invalid());
         Assert::IsTrue(e1.is_trivial());
         Assert::IsFalse(e1.is_canonical());
         Assert::AreEqual(edge::invalid, e1);
         Assert::IsTrue(edge::invalid == e1);
         Assert::IsTrue(e1 == e1);
         Assert::IsFalse(e1 < e1);

         const edge e2(point(1., 2.), point(-3., -4.));
         Assert::AreEqual(1., e2.p1.x);
         Assert::AreEqual(2., e2.p1.y);
         Assert::AreEqual(-3., e2.p2.x);
         Assert::AreEqual(-4., e2.p2.y);

         Assert::AreNotEqual(e1, e2);
         Assert::AreNotEqual(edge::invalid, e2);
         Assert::IsFalse(e2.is_invalid());
         Assert::IsFalse(e2.is_trivial());
         Assert::IsFalse(e2.is_canonical());
         Assert::IsTrue(e1 != e2);
         Assert::IsTrue(e2 == e2);
         Assert::IsTrue(edge::invalid != e2);
         Assert::IsFalse(e1 < e2);
         Assert::IsFalse(e2 < e2);
         Assert::IsFalse(edge::invalid < e2);

         const edge e3(point(2., 1.), point(5., 7.));
         Assert::AreEqual(2., e3.p1.x);
         Assert::AreEqual(1., e3.p1.y);
         Assert::AreEqual(5., e3.p2.x);
         Assert::AreEqual(7., e3.p2.y);

         Assert::AreNotEqual(e1, e3);
         Assert::AreNotEqual(e2, e3);
         Assert::AreNotEqual(edge::invalid, e3);
         Assert::IsFalse(e3.is_invalid());
         Assert::IsFalse(e3.is_trivial());
         Assert::IsTrue(e3.is_canonical());
         Assert::IsTrue(e1 != e3);
         Assert::IsTrue(e2 != e3);
         Assert::IsTrue(e3 == e3);
         Assert::IsTrue(edge::invalid != e3);
         Assert::IsFalse(e1 < e3);
         Assert::IsTrue(e2 < e3);
         Assert::IsFalse(e3 < e3);
         Assert::IsFalse(edge::invalid < e3);

         const edge e4(e3);
         Assert::AreEqual(2., e4.p1.x);
         Assert::AreEqual(1., e4.p1.y);
         Assert::AreEqual(5., e4.p2.x);
         Assert::AreEqual(7., e4.p2.y);

         Assert::AreNotEqual(e1, e4);
         Assert::AreNotEqual(e2, e4);
         Assert::AreEqual(e3, e4);
         Assert::AreNotEqual(edge::invalid, e4);
         Assert::IsFalse(e4.is_invalid());
         Assert::IsFalse(e4.is_trivial());
         Assert::IsTrue(e4.is_canonical());
         Assert::IsTrue(e1 != e4);
         Assert::IsTrue(e2 != e4);
         Assert::IsTrue(e3 == e4);
         Assert::IsTrue(e4 == e4);
         Assert::IsTrue(edge::invalid != e4);
         Assert::IsFalse(e1 < e4);
         Assert::IsTrue(e2 < e4);
         Assert::IsFalse(e3 < e4);
         Assert::IsFalse(e4 < e4);
         Assert::IsFalse(edge::invalid < e4);

         const edge e5(e2.canonical());
         Assert::AreEqual(-3., e5.p1.x);
         Assert::AreEqual(-4., e5.p1.y);
         Assert::AreEqual(1., e5.p2.x);
         Assert::AreEqual(2., e5.p2.y);

         Assert::AreNotEqual(e1, e5);
         Assert::AreNotEqual(e2, e5);
         Assert::AreNotEqual(e3, e5);
         Assert::AreNotEqual(e4, e5);
         Assert::AreNotEqual(edge::invalid, e5);
         Assert::IsFalse(e5.is_invalid());
         Assert::IsFalse(e5.is_trivial());
         Assert::IsTrue(e5.is_canonical());
         Assert::IsTrue(e1 != e5);
         Assert::IsTrue(e2 != e5);
         Assert::IsTrue(e3 != e5);
         Assert::IsTrue(e4 != e5);
         Assert::IsTrue(e5 == e5);
         Assert::IsTrue(e2.canonical() == e5);
         Assert::IsTrue(edge::invalid != e5);
         Assert::IsFalse(e1 < e5);
         Assert::IsFalse(e2 < e5);
         Assert::IsFalse(e3 < e5);
         Assert::IsFalse(e4 < e5);
         Assert::IsFalse(e5 < e5);
         Assert::IsTrue(e5 < e2);
         Assert::IsTrue(e5 < e3);
         Assert::IsTrue(e5 < e4);
         Assert::IsFalse(edge::invalid < e5);

         Assert::AreEqual(point::origin(), edge::unit_x().p1);
         Assert::AreEqual(point::unit_x(), edge::unit_x().p2);

         Assert::AreEqual(point::origin(), edge::unit_y().p1);
         Assert::AreEqual(point::unit_y(), edge::unit_y().p2);

         const edge e6 = edge::lowest_edge(e2.p1);
         Assert::IsTrue(e6 < e2);

         const edge e7 = edge::highest_edge(e2.p1);
         Assert::IsTrue(e2 < e7);

         edge e8(point(1., 2.), point(3., 4.));
         edge e9(point(5., 6.), point(7., 8.));
         Assert::AreEqual(point(1., 2.), e8.p1);
         Assert::AreEqual(point(3., 4.), e8.p2);
         Assert::AreEqual(point(5., 6.), e9.p1);
         Assert::AreEqual(point(7., 8.), e9.p2);

         e8.swap(e9);
         Assert::AreEqual(point(5., 6.), e8.p1);
         Assert::AreEqual(point(7., 8.), e8.p2);
         Assert::AreEqual(point(1., 2.), e9.p1);
         Assert::AreEqual(point(3., 4.), e9.p2);

         const edge e10 = e8.twin();
         Assert::AreEqual(point(7., 8.), e10.p1);
         Assert::AreEqual(point(5., 6.), e10.p2);

         const edge e11(point(1., 1.), point(1., 1.));
         Assert::AreEqual(1., e11.p1.x);
         Assert::AreEqual(1., e11.p1.y);
         Assert::AreEqual(1., e11.p2.x);
         Assert::AreEqual(1., e11.p2.y);

         Assert::AreNotEqual(e1, e11);
         Assert::AreNotEqual(e2, e11);
         Assert::AreNotEqual(e3, e11);
         Assert::AreNotEqual(e4, e11);
         Assert::AreNotEqual(edge::invalid, e11);
         Assert::IsFalse(e11.is_invalid());
         Assert::IsTrue(e11.is_trivial());
         Assert::IsFalse(e11.is_canonical());
         Assert::IsFalse(e11.twin().is_canonical());
         Assert::IsTrue(e1 != e11);
         Assert::IsTrue(e2 != e11);
         Assert::IsTrue(e3 != e11);
         Assert::IsTrue(e4 != e11);
         Assert::IsTrue(e11 == e11);
         Assert::IsTrue(e11.twin() == e11);
         Assert::IsTrue(edge::invalid != e11);
         Assert::IsFalse(e1 < e11);
         Assert::IsFalse(e2 < e11);
         Assert::IsFalse(e3 < e11);
         Assert::IsFalse(e4 < e11);
         Assert::IsFalse(e11 < e11);
         Assert::IsTrue(e11 < e2);
         Assert::IsTrue(e11 < e3);
         Assert::IsTrue(e11 < e4);
         Assert::IsFalse(edge::invalid < e11);
      }

      TEST_METHOD(edge_angles)
      {
         Assert::AreEqual(0., edge::unit_x().angle());
         Assert::AreEqual(PI / 2, edge::unit_y().angle());
         Assert::AreEqual(PI / 2, edge::unit_x().angle(edge::unit_y()));
         Assert::AreEqual(3 * PI / 2, edge::unit_y().angle(edge::unit_x()));
      }

      TEST_METHOD(edge_transform)
      {
         {
            const edge e1(point(1., 2.), point(-3., -4.));

            Assert::AreEqual(edge(point(-2., 1.), point(4., -3.)), e1.apply(transform::rotate(PI / 2)));
            Assert::AreEqual(edge(point(-1., -2.), point(3., 4.)), e1.apply(transform::rotate(PI)));
            Assert::AreEqual(edge(point(2., -1.), point(-4., 3.)), e1.apply(transform::rotate(3 * PI / 2)));
            Assert::AreEqual(edge(point(1., 2.), point(-3., -4.)), e1.apply(transform::rotate(2 * PI)));
         }

         {
            Assert::AreEqual(edge(point::origin(), point(0., 1.)), edge::unit_x().apply(transform::rotate(PI / 2)));
            Assert::AreEqual(edge(point::origin(), point(-1., 0.)), edge::unit_x().apply(transform::rotate(PI)));
            Assert::AreEqual(edge(point::origin(), point(0., -1.)), edge::unit_x().apply(transform::rotate(3 * PI / 2)));
            Assert::AreEqual(edge::unit_x(), edge::unit_x().apply(transform::rotate(2 * PI)));

            Assert::AreEqual(edge(point::origin(), point(std::sqrt(2.) / 2., std::sqrt(2.) / 2.)), edge::unit_x().apply(transform::rotate(PI / 4)));
         }

         {
            const edge e1(point(1., 2.), point(-3., -4.));

            Assert::AreEqual(edge(point(3., 5.), point(-1., -1.)), e1.apply(transform::translate(2., 3.)));
            Assert::AreEqual(edge(point(-2., -5.), point(-6., -11.)), e1.apply(transform::translate(point(-3., -7.))));
         }

         {
            const edge e1(point(1., 2.), point(-3., -4.));

            Assert::AreEqual(edge(point(2., 4.), point(-6., -8.)), e1.apply(transform::scale(2.)));
            Assert::AreEqual(edge(point(-3., -14.), point(9., 28.)), e1.apply(transform::scale(-3., -7.)));
         }

      }
   };
}