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
      // A widget drawing_canvas working with a map.

      map_canvas::map_canvas(QWidget* parent)
      : drawing_canvas(parent)
      {
      }

      void map_canvas::draw(drawing& drw)
      {
         draw_map(drw);
         drawing_canvas::draw(drw);
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
