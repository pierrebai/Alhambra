#include <dak/tiling_ui_qt/mosaic_canvas.h>
#include <dak/tiling_ui_qt/drawing.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using ui::color_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a mosaic.

      mosaic_canvas_t::mosaic_canvas_t(QWidget* parent)
      : drawing_canvas_t(parent)
      , mosaic(nullptr)
      {
      }

      void mosaic_canvas_t::draw(ui::drawing_t& drw)
      {
         // Make it so we can see 9 instances (3x3) of the tiling.
         draw_tiling(drw, mosaic, ui::color_t::black(), 3);
         drawing_canvas_t::draw(drw);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
