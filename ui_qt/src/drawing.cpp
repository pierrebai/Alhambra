#include <dak/ui_qt/drawing.h>
#include <dak/ui_qt/convert.h>

#include <dak/tiling/mosaic.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace ui_qt
   {
      using geometry::transform;

      QPen drawing::get_pen() const
      {
         const auto& co = get_color();
         const auto& strk = get_stroke();
         return QPen(convert(co), strk.width, Qt::SolidLine, convert(strk.cap), convert(strk.join));
      }

      QBrush drawing::get_brush() const
      {
         const auto& co = get_color();
         return QBrush(convert(co));
      }

      static void clear_background(dak::ui::drawing& drw)
      {
         drw.set_color(color::white());
         drw.fill_polygon(polygon::from_rect(drw.get_bounds()).apply(drw.get_transform().invert()));
      }

      static bool begin_drawing_tiling(dak::ui::drawing& drw, const tiling::tiling& tiling, const color& co, int copy_count)
      {
         geometry::rect tiling_bounds = tiling.bounds();
         if (tiling_bounds.is_invalid())
            return false;

         double ratio = std::max(drw.get_bounds().width / tiling_bounds.width, drw.get_bounds().height / tiling_bounds.height);
         drw.push_transform();
         drw.compose(transform::scale(ratio / copy_count));

         // Draw the tiling in the region.
         drw.set_color(co.is_pale() ? co.percent(80) : co);
         drw.set_stroke(stroke(2.));

         return true;
      }

      static void end_drawing_tiling(dak::ui::drawing& drw)
      {
         drw.set_stroke(stroke(1));
         drw.pop_transform();
      }

      void draw_tiling(dak::ui::drawing& drw, const tiling::tiling& tiling, const color& co, int copy_count)
      {
         clear_background(drw);

         if (!begin_drawing_tiling(drw, tiling, co, copy_count))
            return;

         const geometry::rect region = drw.get_bounds().apply(drw.get_transform().invert());
         geometry::fill(region, tiling.t1, tiling.t2, [&tiling, &drw](int t1, int t2) {
            for (const auto& poly_trfs : tiling.tiles)
            {
               for (const auto& trf : poly_trfs.second)
               {
                  const auto placed_poly = poly_trfs.first.apply(trf).apply(transform::translate(tiling.t1.scale(t1) + tiling.t2.scale(t2)));
                  geometry::point prev = placed_poly.points.back();
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

      void draw_tiling(dak::ui::drawing& drw, const std::shared_ptr<tiling::mosaic>& mosaic, const color& co, int copy_count)
      {
         clear_background(drw);

         if (!mosaic)
            return;

         if (!begin_drawing_tiling(drw, mosaic->tiling, co, copy_count))
            return;

         const tiling::mosaic& mo = *mosaic;
         const geometry::rect region = drw.get_bounds().apply(drw.get_transform().invert());
         geometry::fill(region, mosaic->tiling.t1, mosaic->tiling.t2, [&mo, &drw](int t1, int t2) {
            for (const auto placed_tile : mo.tiling.tiles)
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
                     const auto placed = edge.apply(trf).apply(transform::translate(mo.tiling.t1.scale(t1) + mo.tiling.t2.scale(t2)));
                     drw.draw_line(placed.p1, placed.p2);
                  }
               }
            }
         });

         end_drawing_tiling(drw);
      }

      void draw_layered(dak::ui::drawing& drw, ui::layered* layered)
      {
         drw.set_color(color::white());
         drw.fill_rect(drw.get_bounds().apply(drw.get_transform().invert()));

         if (!layered)
            return;

         drw.set_color(color::black());
         drw.set_stroke(stroke(1.2));
         layered->draw(drw);
         drw.set_stroke(stroke(1));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
