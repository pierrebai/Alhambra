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
         draw_layered(layered_drawing);
         layered_drawing.painter = nullptr;

         canvas::paint(painter);
      }

      void layered_canvas::draw_layered(dak::ui::drawing& drw) const
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
