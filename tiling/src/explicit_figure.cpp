#include <dak/tiling/explicit_figure.h>

#include <dak/utility/text.h>

#include <sstream>

namespace dak
{
   namespace tiling
   {
      using utility::L;

      std::wstring explicit_figure::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Explicit Figure") << L" " << get_map().all().size();
         return ss.str();
      }

      std::shared_ptr<figure> explicit_figure::clone() const
      {
         return std::make_shared<explicit_figure>(*this);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
