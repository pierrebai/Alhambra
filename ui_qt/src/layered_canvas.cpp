#include <dak/ui_qt/layered_canvas.h>

#include <QtGui/qpainter.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::color;
      using ui::stroke;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a layered.

      layered_canvas::layered_canvas(QWidget* parent)
      : drawing_canvas(parent)
      , layered(nullptr)
      {
      }

      void layered_canvas::draw(drawing& drw)
      {
         draw_layered(drw, layered);
         drawing_canvas::draw(drw);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
