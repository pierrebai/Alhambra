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
      // A widget canvas working with a mosaic.

      mosaic_canvas::mosaic_canvas(QWidget* parent)
      : canvas(parent, mosaic_drawing)
      , mosaic(nullptr)
      {
      }

      void mosaic_canvas::paint(QPainter& painter)
      {
         mosaic_drawing.painter = &painter;
         // Make it so we can see 9 instances (3x3) of the tiling.
         draw_tiling(mosaic_drawing, mosaic, color::black(), 3);
         mosaic_drawing.painter = nullptr;

         canvas::paint(painter);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
