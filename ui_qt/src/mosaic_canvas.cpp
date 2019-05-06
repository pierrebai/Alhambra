#include <dak/ui_qt/mosaic_canvas.h>
#include <dak/ui_qt/convert.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::color;
      using ui::stroke;
      using ui::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a mosaic.

      mosaic_canvas::mosaic_canvas(QWidget* parent)
      : drawing_canvas(parent)
      , mosaic(nullptr)
      {
      }

      void mosaic_canvas::draw(drawing& drw)
      {
         // Make it so we can see 9 instances (3x3) of the tiling.
         draw_tiling(drw, mosaic, color::black(), 3);
         drawing_canvas::draw(drw);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
