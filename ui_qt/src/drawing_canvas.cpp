#include <dak/ui_qt/drawing_canvas.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::color;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a transformable drawing.

      drawing_canvas::drawing_canvas(QWidget* parent)
      : drawing_canvas(parent, painter_trf_drawing)
      {
      }

      drawing_canvas::drawing_canvas(QWidget* parent, ui::transformable& trfable)
      : canvas(parent, trfable)
      {
         transformer.manipulated = &trfable;
      }

      void drawing_canvas::paint(QPainter& painter)
      {
         painter_trf_drawing.painter = &painter;
         draw(painter_trf_drawing);
         painter_trf_drawing.painter = nullptr;

         canvas::paint(painter);
      }

      void drawing_canvas::draw(drawing&)
      {
         // Nothing.
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
