#include <dak/utility/undo_stack.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::utility;

namespace utility_tests
{		
	TEST_CLASS(undo_stack_tests)
	{
	public:
		
		TEST_METHOD(undo_stack_undo_redo)
		{
         undo_stack undo;

         Assert::AreEqual<size_t>(0, undo.contents().size());

         struct data
         {
            data(double a) : a(a), a_squared(a*a) { }

            double a = 0.;
            double a_squared = 0.;
         };

         data my_data(7.);

         Assert::AreEqual(7., my_data.a);
         Assert::AreEqual(49., my_data.a_squared);

         undo.commit(
         {
            my_data,
            [&](std::any& d)
            {
               std::any_cast<data&>(d).a_squared = 0.;
            },
            [&my_data=my_data](const std::any& d)
            {
               my_data = std::any_cast<const data&>(d);
               my_data.a_squared = my_data.a * my_data.a;
            }
         });

         Assert::AreEqual(7., my_data.a);
         Assert::AreEqual(49., my_data.a_squared);

         Assert::AreEqual<size_t>(1, undo.contents().size());
         Assert::AreEqual(7., std::any_cast<const data&>(undo.contents().back().data).a);
         Assert::AreEqual(0., std::any_cast<const data&>(undo.contents().back().data).a_squared);

         my_data = data(9.);

         undo.commit(
         {
            my_data,
            [&](std::any& d)
            {
               std::any_cast<data&>(d).a_squared = 0.;
            },
            [&my_data=my_data](const std::any& d)
            {
               my_data = std::any_cast<const data&>(d);
               my_data.a_squared = my_data.a * my_data.a;
            }
         });

         Assert::AreEqual(9., my_data.a);
         Assert::AreEqual(81., my_data.a_squared);

         Assert::AreEqual<size_t>(2, undo.contents().size());
         Assert::AreEqual(9., std::any_cast<const data&>(undo.contents().back().data).a);
         Assert::AreEqual(0., std::any_cast<const data&>(undo.contents().back().data).a_squared);

         undo.undo();

         Assert::AreEqual(7., my_data.a);
         Assert::AreEqual(49., my_data.a_squared);

         undo.redo();

         Assert::AreEqual(9., my_data.a);
         Assert::AreEqual(81., my_data.a_squared);
      }

	};
}