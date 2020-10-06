#include <dak/tiling/figure.h>

namespace dak
{
   namespace tiling
   {
      const edges_map_t& figure_t::get_map() const
      {
         if (is_cache_valid())
            return cached_map;

         cached_map = edges_map_t();

         build_map();

         update_cached_values();

         return cached_map;
      }

      bool figure_t::is_cache_valid() const
      {
         return cached_map.all().size() > 0;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
