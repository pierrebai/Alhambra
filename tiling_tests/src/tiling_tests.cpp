#include <dak/tiling/tiling.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;
using namespace dak::tiling;

namespace tiling_tests
{		
	TEST_CLASS(tiling_tests)
	{
	public:
		
		TEST_METHOD(tiling_constructor)
		{
         const translation_tiling_t t1;

         Assert::IsTrue(t1.t1.is_invalid());
         Assert::IsTrue(t1.t2.is_invalid());
         Assert::IsTrue(t1.tiles.empty());
         Assert::IsTrue(t1.name.empty());

         translation_tiling_t t2(L"tiling 2", point_t(1., 2.), point_t(3., 4.));

         Assert::IsFalse(t2.t1.is_invalid());
         Assert::IsFalse(t2.t2.is_invalid());
         Assert::IsTrue(t2.tiles.empty());
         Assert::IsFalse(t2.name.empty());

         t2.tiles[polygon_t({ point_t(1., 2.), point_t(3., 4.), point_t(5., 6.) })].emplace_back(transform_t::identity());
         t2.tiles[polygon_t({ point_t(1., 2.), point_t(3., 4.), point_t(5., 6.) })].emplace_back(transform_t::identity());

         Assert::IsFalse(t2.tiles.empty());
         Assert::IsFalse(t2.tiles.empty());
         Assert::IsFalse(t2.tiles.begin()->second.empty());
      }

	};
}
