#include <dak/ui_qt/tiling_canvas.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::color;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a tiling.

      tiling_canvas::tiling_canvas(QWidget* parent)
      : drawing_canvas(parent)
      , tiling()
      {
      }

      void tiling_canvas::draw(drawing& drw)
      {
         // Make it so we can see 9 instances (3x3) of the tiling.
         draw_tiling(drw, tiling, color::black(), 3);
         drawing_canvas::draw(drw);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
