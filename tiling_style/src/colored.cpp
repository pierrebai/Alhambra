#include <dak/tiling_style/colored.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_style
   {
      void colored::make_similar(const layer& other)
      {
         style::make_similar(other);

         if (const colored* other_colored = dynamic_cast<const colored*>(&other))
         {
            color = other_colored->color;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
