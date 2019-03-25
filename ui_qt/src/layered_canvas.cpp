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
      // A widget canvas working with a layered.

      layered_canvas::layered_canvas(QWidget* parent)
      : canvas(parent)
      , layered(nullptr)
      {
      }

      void layered_canvas::paint(QPainter& painter)
      {
         layered_drawing.painter = &painter;
         draw_layered(layered_drawing, layered);
         layered_drawing.painter = nullptr;

         canvas::paint(painter);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
