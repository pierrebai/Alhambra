#include <dak/tiling/radial_figure.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/transform.h>

namespace dak
{
   namespace tiling
   {
      using geometry::transform;
      using geometry::PI;

      void radial_figure::build_map() const
      {
         const map unit = build_unit();

         for (int i = 0; i < n; ++i)
         {
            // Note: we could speed-up by only applying a base rotation
            //       multiple times to a non-const unit, but we would
            //       accumulate imprecisions for large n.
            cached_map.merge_non_overlapping(unit.apply(transform::rotate(2 * PI * i / n)));
            //cached_map.merge(unit.apply(transform::rotate(2 * PI * i / n)));
         }
      }

      bool radial_figure::is_cache_valid() const
      {
         return cached_n_last_build_unit == n
             && figure::is_cache_valid();
      }

      void radial_figure::update_cached_values() const
      {
         cached_n_last_build_unit = n;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
