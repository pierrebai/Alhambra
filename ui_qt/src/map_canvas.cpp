#include <dak/ui_qt/map_canvas.h>

#include <QtGui/qpainter.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::color;
      using ui::stroke;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a map.

      map_canvas::map_canvas(QWidget* parent)
      : canvas(parent)
      {
      }

      void map_canvas::paint(QPainter& painter)
      {
         map_drawing.painter = &painter;
         draw_map(map_drawing);
         map_drawing.painter = nullptr;

         canvas::paint(painter);
      }

      void map_canvas::draw_map(dak::ui::drawing& drw) const
      {
         geometry::rect bounds = drw.get_bounds().apply(drw.get_transform().invert());
         drw.set_color(color::white());
         drw.fill_rect(bounds);

         drw.set_color(color::black());
         drw.set_stroke(stroke(1.2));
         for (const auto& edge : map.all())
            if (edge.is_canonical())
               drw.draw_line(edge.p1, edge.p2);
         drw.set_stroke(stroke(1));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
