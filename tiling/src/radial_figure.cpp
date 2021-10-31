#include <dak/tiling/radial_figure.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/transform.h>

namespace dak
{
   namespace tiling
   {
      using geometry::transform_t;
      using geometry::PI;

      void radial_figure_t::build_map() const
      {
         const edges_map_t unit = build_unit();

         my_cached_map.reserve(unit.all().size() * n);

         my_cached_map.begin_merge_non_overlapping();
         for (int i = 0; i < n; ++i)
         {
            // Note: we could speed-up by only applying a base rotation
            //       multiple times to a non-const unit, but we would
            //       accumulate imprecisions for large n.
            my_cached_map.merge_non_overlapping(unit.apply(transform_t::rotate(2 * PI * i / n)));
            //my_cached_map.merge(unit.apply(transform::rotate(2 * PI * i / n)));
         }
         my_cached_map.end_merge_non_overlapping();
      }

      bool radial_figure_t::is_cache_valid() const
      {
         return my_cached_n_last_build_unit == n
             && figure_t::is_cache_valid();
      }

      void radial_figure_t::update_cached_values() const
      {
         my_cached_n_last_build_unit = n;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
