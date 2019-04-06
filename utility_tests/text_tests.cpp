#include <dak/utility/text.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::utility;

namespace geometry_tests
{
   TEST_CLASS(utility_text_tests)
   {
   public:

      TEST_METHOD(utility_text_convert)
      {
         Assert::AreEqual(std::wstring(L"Hello"), convert("Hello"));
         Assert::AreEqual(std::wstring(L"Hello"), convert(std::string("Hello")));
      }

      TEST_METHOD(utility_text_localisation)
      {
         Assert::AreEqual(L"Star", L::t(L"Star"));
         L::set_english();
         Assert::AreEqual(L"Star", L::t(L"Star"));
         L::set_french();
         Assert::AreEqual(L"Étoile", L::t(L"Star"));
      }
   };
}