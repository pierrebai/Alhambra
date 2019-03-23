#include <dak/tiling/figure.h>

namespace dak
{
   namespace tiling
   {
      const map& figure::get_map() const
      {
         if (is_cache_valid())
            return cached_map;

         cached_map = map();

         build_map();

         update_cached_values();

         return cached_map;
      }

      bool figure::is_cache_valid() const
      {
         return cached_map.canonicals().size() > 0;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
