#include <dak/ui_qt/mosaic_canvas.h>
#include <dak/ui_qt/convert.h>

#include <dak/geometry/point_utility.h>

#include <QtGui/qpainter.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::color;
      using ui::stroke;
      using ui::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a mosaic.

      mosaic_canvas::mosaic_canvas(QWidget* parent)
      : canvas(parent, mosaic_drawing)
      , mosaic(nullptr)
      {
      }

      void mosaic_canvas::paint(QPainter& painter)
      {
         mosaic_drawing.painter = &painter;
         draw_tiling(mosaic_drawing);
         mosaic_drawing.painter = nullptr;

         canvas::paint(painter);
      }

      void mosaic_canvas::draw_tiling(dak::ui::drawing& drw) const
      {
         drw.set_color(color::white());
         drw.fill_rect(drw.get_bounds().apply(drw.get_transform().invert()));

         if (!mosaic)
            return;

         // Make it so we can see 9 instances (3x3) of the tiling.
         geometry::rect tiling_bounds = mosaic->tiling.bounds();
         if (tiling_bounds.is_invalid())
            return;

         double ratio = std::max(drw.get_bounds().width / tiling_bounds.width, drw.get_bounds().height / tiling_bounds.height);
         drw.set_transform(transform::scale(ratio / 3.));

         // Draw the tiling in the region.
         drw.set_color(color::black());
         drw.set_stroke(stroke(1.2));

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
                  for (const auto& edge : map.canonicals())
                  {
                     const auto placed = edge.apply(trf).apply(transform::translate(mo.tiling.t1.scale(t1) + mo.tiling.t2.scale(t2)));
                     drw.draw_line(placed.p1, placed.p2);
                  }
               }
            }
         });

         drw.set_stroke(stroke(1));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
