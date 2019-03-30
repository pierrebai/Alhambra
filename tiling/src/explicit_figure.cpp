#include <dak/tiling/explicit_figure.h>

#include <dak/geometry/utility.h>

#include <sstream>

namespace dak
{
   namespace tiling
   {
      using geometry::L;

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
