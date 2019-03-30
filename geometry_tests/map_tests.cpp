#include <dak/geometry/map.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/constants.h>

#include <cmath>

#include "CppUnitTest.h"

#include "test_helpers.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{
   TEST_CLASS(map_tests)
   {
   public:

      static size_t range_size(const map::range& r)
      {
         return r.second - r.first;
      }

      TEST_METHOD(map_constructors)
      {
         const map m1;

         Assert::AreEqual<size_t>(0, m1.all().size());
         Assert::AreEqual<size_t>(0, range_size(m1.outbounds(point::origin())));

         Assert::IsFalse(m1.contains(point::origin()));
         Assert::IsFalse(m1.are_connected(point::origin(), point::unit_x()));
         
         Assert::AreEqual<size_t>(0, m1.verify().size());
      }

      TEST_METHOD(map_connections_simple)
      {
         map m1({
             edge(point(1., 2.), point(-3., -4.)),
             edge(point(-3., -4.), point(-3., 3.)),
             edge(point(-3., 3.), point(2., 3.)),
             edge(point(2., 3.), point(1., 2.)),
            });

         Assert::AreEqual<size_t>(8, m1.all().size());
         Assert::AreEqual<size_t>(2, range_size(m1.outbounds(point(1., 2.))));

         Assert::IsTrue(m1.contains(point(1., 2.)));
         Assert::IsTrue(m1.contains(point(-3., -4.)));
         Assert::IsTrue(m1.contains(point(-3., 3.)));
         Assert::IsTrue(m1.contains(point(2., 3.)));

         Assert::IsTrue(m1.are_connected(point(1., 2.), point(-3., -4.)));
         Assert::IsTrue(m1.are_connected(point(-3., -4.), point(-3., 3.)));
         Assert::IsTrue(m1.are_connected(point(-3., 3.), point(2., 3.)));
         Assert::IsTrue(m1.are_connected(point(2., 3.), point(1., 2.)));

         Assert::IsFalse(m1.are_connected(point(1., 2.), point(-3., 3.)));
         Assert::IsFalse(m1.are_connected(point(-3., -4.), point(2., 3.)));

         Assert::AreEqual<size_t>(0, m1.verify().size());
      }

      TEST_METHOD(map_connections_cross)
      {
         map m1({
             edge(point(-2., 0.), point(2., 0.)),
             edge(point(0., -3.), point(0., 3.)),
            });

         Assert::AreEqual<size_t>(8, m1.all().size());
         Assert::AreEqual<size_t>(1, range_size(m1.outbounds(point(2., 0.))));

         Assert::IsTrue(m1.contains(point(-2.,  0.)));
         Assert::IsTrue(m1.contains(point( 2.,  0.)));
         Assert::IsTrue(m1.contains(point( 0.,  3.)));
         Assert::IsTrue(m1.contains(point( 0., -3.)));
         Assert::IsTrue(m1.contains(point( 0.,  0.)));

         Assert::IsTrue(m1.are_connected(point(-2.,  0.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point( 2.,  0.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point( 0.,  3.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point( 0., -3.), point(0., 0.)));

         Assert::IsFalse(m1.are_connected(point(-2.,  0.), point(2., 0.)));
         Assert::IsFalse(m1.are_connected(point( 0., -3.), point(0., 3.)));

         Assert::AreEqual<size_t>(0, m1.verify().size());
      }

      TEST_METHOD(map_insert_remove)
      {
         map m1;
         
         m1.insert(edge(point(-2., 0.), point(2., 0.)));

         Assert::AreEqual<size_t>(2, m1.all().size());

         Assert::AreEqual<size_t>(1, range_size(m1.outbounds(point(-2., 0.))));

         Assert::IsTrue(m1.contains(point(-2., 0.)));
         Assert::IsTrue(m1.contains(point( 2., 0.)));

         Assert::IsTrue( m1.are_connected(point(-2., 0.), point( 2., 0.)));
         Assert::IsTrue( m1.are_connected(point( 2., 0.), point(-2., 0.)));
         Assert::IsFalse(m1.are_connected(point( 2., 0.), point( 0., 0.)));

         m1.insert(edge(point(0., -3.), point(0., 3.)));

         Assert::AreEqual<size_t>(0, m1.verify().size());

         Assert::AreEqual<size_t>(8, m1.all().size());

         Assert::AreEqual<size_t>(1, range_size(m1.outbounds(point(0., 3.))));

         Assert::AreEqual<size_t>(4, range_size(m1.outbounds(point(0., 0.))));

         Assert::IsTrue(m1.contains(point(-2.,  0.)));
         Assert::IsTrue(m1.contains(point( 2.,  0.)));
         Assert::IsTrue(m1.contains(point( 0.,  3.)));
         Assert::IsTrue(m1.contains(point( 0., -3.)));
         Assert::IsTrue(m1.contains(point( 0.,  0.)));

         Assert::IsTrue(m1.are_connected(point(-2.,  0.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point( 2.,  0.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point( 0.,  3.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point( 0., -3.), point(0., 0.)));

         Assert::IsFalse(m1.are_connected(point(-2.,  0.), point(2., 0.)));
         Assert::IsFalse(m1.are_connected(point( 0., -3.), point(0., 3.)));

         m1.remove(edge(point(0., 0.), point(0., 3.)));

         Assert::AreEqual<size_t>(0, m1.verify().size());

         Assert::AreEqual<size_t>(6, m1.all().size());

         Assert::AreEqual<size_t>(0, range_size(m1.outbounds(point(0., 3.))));

         Assert::AreEqual<size_t>(1, range_size(m1.outbounds(point(0., -3.))));

         Assert::AreEqual<size_t>(3, range_size(m1.outbounds(point(0., 0.))));

         Assert::IsTrue(m1.contains(point(-2., 0.)));
         Assert::IsTrue(m1.contains(point(2., 0.)));
         Assert::IsFalse(m1.contains(point(0., 3.)));
         Assert::IsTrue(m1.contains(point(0., -3.)));
         Assert::IsTrue(m1.contains(point(0., 0.)));

         Assert::IsTrue(m1.are_connected(point(-2., 0.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point(2., 0.), point(0., 0.)));
         Assert::IsFalse(m1.are_connected(point(0., 3.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point(0., -3.), point(0., 0.)));

         Assert::IsFalse(m1.are_connected(point(-2., 0.), point(2., 0.)));
         Assert::IsFalse(m1.are_connected(point(0., -3.), point(0., 3.)));

         m1.remove(point(2., 0.));

         Assert::AreEqual<size_t>(0, m1.verify().size());

         Assert::AreEqual<size_t>(4, m1.all().size());

         Assert::AreEqual<size_t>(0, range_size(m1.outbounds(point(2., 0.))));

         Assert::AreEqual<size_t>(1, range_size(m1.outbounds(point(-2., 0.))));

         Assert::IsTrue(m1.contains(point(-2., 0.)));
         Assert::IsFalse(m1.contains(point(2., 0.)));
         Assert::IsFalse(m1.contains(point(0., 3.)));
         Assert::IsTrue(m1.contains(point(0., -3.)));
         Assert::IsTrue(m1.contains(point(0., 0.)));

         Assert::IsTrue(m1.are_connected(point(-2., 0.), point(0., 0.)));
         Assert::IsFalse(m1.are_connected(point(2., 0.), point(0., 0.)));
         Assert::IsFalse(m1.are_connected(point(0., 3.), point(0., 0.)));
         Assert::IsTrue(m1.are_connected(point(0., -3.), point(0., 0.)));

         Assert::IsFalse(m1.are_connected(point(-2., 0.), point(2., 0.)));
         Assert::IsFalse(m1.are_connected(point(0., -3.), point(0., 3.)));
      }

      TEST_METHOD(map_singleton_transform)
      {
         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::rotate(PI / 2));
            Assert::IsTrue(m2.contains(edge(point(-2., 1.), point(4., -3.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::rotate(PI));
            Assert::IsTrue(m2.contains(edge(point(-1., -2.), point(3., 4.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::rotate(3 * PI / 2));
            Assert::IsTrue(m2.contains(edge(point(2., -1.), point(-4., 3.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::rotate(2 * PI));
            Assert::IsTrue(m2.contains(edge(point(1., 2.), point(-3., -4.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point::origin(), point::unit_x()) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::rotate(PI / 4));
            Assert::IsTrue(m2.contains(edge(point::origin(), point(std::sqrt(2.) / 2., std::sqrt(2.) / 2.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::translate(2., 3.));
            Assert::IsTrue(m2.contains(edge(point(3., 5.), point(-1., -1.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::translate(point(-3., -7.)));
            Assert::IsTrue(m2.contains(edge(point(-2., -5.), point(-6., -11.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::scale(2.));
            Assert::IsTrue(m2.contains(edge(point(2., 4.), point(-6., -8.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            map m1({ edge(point(1., 2.), point(-3., -4.)) });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::scale(-3., -7.));
            Assert::IsTrue(m2.contains(edge(point(-3., -14.), point(9., 28.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }
      }

      TEST_METHOD(map_crossing_edges_transform)
      {
         {
            const map m1({
                edge(point(-2., 0.), point(2., 0.)),
                edge(point(0., -3.), point(0., 3.)),
                   });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::rotate(PI / 2));
            Assert::IsTrue(m2.contains(edge(point(-3.,  0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 3.,  0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 0., -2.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 0.,  2.), point(0., 0.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            const map m1({
                edge(point(-2., 0.), point(2., 0.)),
                edge(point(0., -3.), point(0., 3.)),
                   });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::rotate(PI));
            Assert::IsTrue(m2.contains(edge(point(-2., 0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point(2., 0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point(0., -3.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point(0., 3.), point(0., 0.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            const map m1({
                edge(point(-2., 0.), point(2., 0.)),
                edge(point(0., -3.), point(0., 3.)),
                   });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::translate(-7, 5.));
            Assert::IsTrue(m2.contains(edge(point(-9., 5.), point(-7., 5.))));
            Assert::IsTrue(m2.contains(edge(point(-5., 5.), point(-7., 5.))));
            Assert::IsTrue(m2.contains(edge(point(-7., 2.), point(-7., 5.))));
            Assert::IsTrue(m2.contains(edge(point(-7., 8.), point(-7., 5.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            const map m1({
                edge(point(-2., 0.), point(2., 0.)),
                edge(point(0., -3.), point(0., 3.)),
                   });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::scale(3.));
            Assert::IsTrue(m2.contains(edge(point(-6.,  0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 6.,  0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 0., -9.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 0.,  9.), point(0., 0.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }

         {
            const map m1({
                edge(point(-2., 0.), point(2., 0.)),
                edge(point(0., -3.), point(0., 3.)),
                   });

            Assert::AreEqual<size_t>(0, m1.verify().size());

            map m2 = m1.apply(transform::scale(0.5, -2.));
            Assert::IsTrue(m2.contains(edge(point(-1.,  0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 1.,  0.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 0., -6.), point(0., 0.))));
            Assert::IsTrue(m2.contains(edge(point( 0.,  6.), point(0., 0.))));

            Assert::AreEqual<size_t>(0, m2.verify().size());
         }
      }

      TEST_METHOD(map_small_merge)
      {
         map_merge(100);
      }

      //#define DAK_GEOMETRY_SLOW_MAP_TESTS

      #ifdef DAK_GEOMETRY_SLOW_MAP_TESTS

      TEST_METHOD(map_medium_merge_333ms)
      {
         map_merge(400);
      }

      TEST_METHOD(map_big_merge_2s)
      {
         map_merge(1000);
      }

      TEST_METHOD(map_very_big_merge_9s)
      {
         map_merge(2000);
      }

      #endif

   private:
      void map_merge(int count)
      {
         map horiz;
         for (int i = 1; i <= count; ++i)
         {
            horiz.insert(edge(point(-count, i / 2.), point(count, i / 2.)));
         }
         Assert::AreEqual<size_t>(count * 2, horiz.all().size());

         Assert::AreEqual<size_t>(0, horiz.verify().size());

         map verti;
         for (int i = 1; i <= count; ++i)
         {
            verti.insert(edge(point(i / 2., -count), point(i / 2., count)));
         }
         Assert::AreEqual<size_t>(count * 2, verti.all().size());

         Assert::AreEqual<size_t>(0, verti.verify().size());

         horiz.merge(verti);

         Assert::AreEqual<size_t>(0, horiz.verify().size());

         Assert::AreEqual<size_t>((count + 1) * count * 2 * 2, horiz.all().size());

         for (int x = 1; x <= count; ++x)
         {
            for (int y = 1; y <= count; ++y)
            {
               Assert::AreEqual<size_t>(4, range_size(horiz.outbounds(point(x / 2., y / 2.))));
            }
         }
      }
   };
}