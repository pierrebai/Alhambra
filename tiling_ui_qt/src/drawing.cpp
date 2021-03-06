#include <dak/tiling_ui_qt/drawing.h>
#include <dak/ui/qt/convert.h>

#include <dak/tiling/mosaic.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using geometry::transform_t;
      using geometry::polygon_t;
      using ui::color_t;
      using ui::stroke_t;
      using tiling::tiling_t;
      using tiling::mosaic_t;

      static void clear_background(dak::ui::drawing_t& drw)
      {
         drw.set_color(color_t::white());
         drw.fill_polygon(polygon_t::from_rect(drw.get_bounds()).apply(drw.get_transform().invert()));
      }

      static bool begin_drawing_tiling(dak::ui::drawing_t& drw, const std::shared_ptr<const tiling_t>& tiling, const color_t& co, int copy_count)
      {
         if (!tiling)
            return false;

         auto tiling_bounds = tiling->bounds();
         if (tiling_bounds.is_invalid())
            return false;

         double ratio = std::max(drw.get_bounds().width / tiling_bounds.width, drw.get_bounds().height / tiling_bounds.height);
         drw.push_transform();
         drw.compose(transform_t::scale(ratio / copy_count));

         // Draw the tiling in the region.
         drw.set_color(co.is_pale() ? co.percent(80) : co);
         drw.set_stroke(stroke_t(2.));

         return true;
      }

      static void end_drawing_tiling(dak::ui::drawing_t& drw)
      {
         drw.set_stroke(stroke_t(1));
         drw.pop_transform();
      }

      void draw_tiling(dak::ui::drawing_t& drw, const std::shared_ptr<const tiling_t>& tiling, const color_t& co, int copy_count)
      {
         clear_background(drw);

         if (!tiling)
            return;

         if (!begin_drawing_tiling(drw, tiling, co, copy_count))
            return;

         const auto region = drw.get_bounds().apply(drw.get_transform().invert());
         tiling->fill(region, [&drw](const tiling_t& tiling, const transform_t& placement) {
            for (const auto& poly_trfs : tiling.tiles)
            {
               for (const auto& trf : poly_trfs.second)
               {
                  const auto placed_poly = poly_trfs.first.apply(trf).apply(placement);
                  geometry::point_t prev = placed_poly.points.back();
                  for (const auto& pt : placed_poly.points)
                  {
                     drw.draw_line(prev, pt);
                     prev = pt;
                  }
               }
            }
         });

         end_drawing_tiling(drw);
      }

      void draw_tiling(dak::ui::drawing_t& drw, const std::shared_ptr<mosaic_t>& mosaic, const color_t& co, int copy_count)
      {
         clear_background(drw);

         if (!mosaic)
            return;

         if (!begin_drawing_tiling(drw, mosaic->tiling, co, copy_count))
            return;

         const tiling::mosaic_t& mo = *mosaic;
         const auto region = drw.get_bounds().apply(drw.get_transform().invert());
         mosaic->tiling->fill(region, [&mo, &drw](const tiling_t& tiling, const transform_t& placement) {
            for (const auto placed_tile : tiling.tiles)
            {
               const auto iter = mo.tile_figures.find(placed_tile.first);
               if (iter == mo.tile_figures.end())
                  continue;
               auto map = iter->second->get_map();
               for (const auto& trf : placed_tile.second)
               {
                  for (const auto& edge : map.all())
                  {
                     if (!edge.is_canonical())
                        continue;
                     const auto placed = edge.apply(trf).apply(placement);
                     drw.draw_line(placed.p1, placed.p2);
                  }
               }
            }
         });

         end_drawing_tiling(drw);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
