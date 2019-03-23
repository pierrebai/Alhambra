#include <dak/geometry/face.h>

#include <cmath>

#include "test_helpers.h"

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;

namespace geometry_tests
{
   TEST_CLASS(face_tests)
   {
   public:

      TEST_METHOD(face_faces_simple)
      {
         map m1({
             edge(point(1., 2.), point(-3., -4.)),
             edge(point(-3., -4.), point(-3., 3.)),
             edge(point(-3., 3.), point(2., 3.)),
             edge(point(2., 3.), point(1., 2.)),
         });

         face::faces white, black, red, exteriors;
         face::make_faces(m1, white, black, red, exteriors);
         Assert::AreEqual<size_t>(2, white.size() + black.size());
         Assert::AreEqual<size_t>(0, exteriors.size());
         Assert::AreEqual<size_t>(4, white[0].points.size());
         Assert::AreEqual<size_t>(4, black[0].points.size());
      }

      TEST_METHOD(face_faces_cross)
      {
         map m1({
             edge(point(-2., 0.), point(2., 0.)),
             edge(point(0., -3.), point(0., 3.)),
         });

         face::faces white, black, red, exteriors;
         face::make_faces(m1, white, black, red, exteriors);
         Assert::AreEqual<size_t>(1, white.size());
         Assert::AreEqual<size_t>(8, white[0].points.size());
      }
   };
}