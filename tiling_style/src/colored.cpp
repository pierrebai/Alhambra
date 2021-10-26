#include <dak/tiling_style/colored.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_style
   {
      void colored_t::make_similar(const layer_t& other)
      {
         style_t::make_similar(other);

         if (const colored_t* other_colored = dynamic_cast<const colored_t*>(&other))
         {
            color = other_colored->color;
         }
      }

      bool colored_t::operator==(const layer_t& other) const
      {
         if (!style_t::operator==(other))
            return false;

         if (const colored_t* other_colored = dynamic_cast<const colored_t*>(&other))
         {
            return color == other_colored->color;
         }

         return false;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
