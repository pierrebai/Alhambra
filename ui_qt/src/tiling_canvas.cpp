#include <dak/ui_qt/tiling_canvas.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::color;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a tiling.

      tiling_canvas::tiling_canvas(QWidget* parent)
      : canvas(parent, tiling_drawing)
      , tiling()
      {
         transformer.manipulated = &tiling_drawing;
      }

      void tiling_canvas::paint(QPainter& painter)
      {
         tiling_drawing.painter = &painter;
         // Make it so we can see 9 instances (3x3) of the tiling.
         draw_tiling(tiling_drawing, tiling, color::black(), 3);
         tiling_drawing.painter = nullptr;

         canvas::paint(painter);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
