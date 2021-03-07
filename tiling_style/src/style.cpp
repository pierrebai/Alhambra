#include <dak/tiling_style/style.h>

#include <dak/tiling/inflation_tiling.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_style
   {
      using geometry::transform_t;
      using geometry::polygon_t;

      void style_t::add_inflation_for_point(const point_t& pt, double inflation)
      {
         const double distance = tiling_center.distance_2(pt);
         inflation_by_distances[distance] = inflation;
      }

      void style_t::set_map(const geometry::edges_map_t& m, const std::shared_ptr<const tiling_t>& t)
      {
         map = m;
         tiling = std::dynamic_pointer_cast<const inflation_tiling_t>(t);

         inflation_by_distances.clear();
         tiling_center = point_t();

         if (!tiling)
            return;

         if (tiling->tiles.size() < 1)
            return;

         if (tiling->tiles.begin()->second.size() < 1)
            return;

         const polygon_t& first_tile = tiling->tiles.begin()->first;
         const transform_t& first_place = *tiling->tiles.begin()->second.begin();
         const double perimeter = first_tile.perimeter();
         if (utility::near_zero(perimeter))
            return;

         tiling_center = tiling->get_center();
         if (tiling_center.is_invalid())
            return;

         add_inflation_for_point(first_tile.center(), 1.);

         tiling->fill_rings(8, [self=this, &center= tiling_center, &first_tile, &first_place, perimeter](const tiling_t& tiling, const transform_t& receive_trf)
         {
            const transform_t total_trf = receive_trf.compose(first_place);
            const auto inflated_tile = first_tile.apply(total_trf);
            const double inflated_peri = inflated_tile.perimeter();
            self->add_inflation_for_point(inflated_tile.center(), inflated_peri / perimeter);
         });
      }

      double style_t::get_width_at(const point_t& pt, double width) const
      {
         if (inflation_by_distances.size() < 2)
            return width;

         const double distance = tiling_center.distance_2(pt);
         const auto pos = inflation_by_distances.lower_bound(distance);
         if (pos == inflation_by_distances.begin())
            return width;

         if (pos == inflation_by_distances.end())
         {
            const auto pred = std::prev(pos);
            const double inflation = pred->second;
            return width * inflation;
         }
         else
         {
            const auto pred = std::prev(pos);
            const double ratio = (distance - pred->first) / (pos->first - pred->first);
            const double inflation = pred->second + (pos->second - pred->second) * ratio;
            return width * inflation;
         }
      }

      void style_t::make_similar(const layer_t& other)
      {
         layer_t::make_similar(other);

         if (const style_t* other_style = dynamic_cast<const style_t*>(&other))
         {
            map = other_style->map;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
