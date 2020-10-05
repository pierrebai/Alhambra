#include <dak/tiling_style/style.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_style
   {
      void style_t::make_similar(const layer& other)
      {
         layer::make_similar(other);

         if (const style_t* other_style = dynamic_cast<const style_t*>(&other))
         {
            map = other_style->map;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
