#include <dak/ui_qt/painter_drawing.h>
#include <dak/ui_qt/convert.h>

#include <QtGui/qpainter.h>
#include <QtGui/qpainterpath.h>

namespace dak
{
   namespace ui_qt
   {
      using geometry::PI;

      painter_drawing& painter_drawing::draw_line(const point& from, const point& to)
      {
         if (!painter)
            return *this;

         const point tf = from.apply(get_transform());
         const point tt = to.apply(get_transform());

         painter->setPen(get_pen());
         painter->drawLine(tf.x, tf.y, tt.x, tt.y);
         return *this;
      }

      painter_drawing& painter_drawing::fill_polygon(const polygon& p)
      {
         if (!painter)
            return *this;

         QPainterPath path;
         path.addPolygon(convert(p.apply(get_transform())));
         path.closeSubpath();
         painter->fillPath(path, get_brush());
         return *this;
      }

      painter_drawing& painter_drawing::draw_polygon(const polygon& p)
      {
         if (!painter)
            return *this;

         QPainterPath path;
         path.addPolygon(convert(p.apply(get_transform())));
         path.closeSubpath();
         painter->setPen(get_pen());
         painter->drawPath(path);
         return *this;
      }

      painter_drawing& painter_drawing::fill_oval(const point& c, double rx, double ry)
      {
         if (!painter)
            return *this;

         const point center = c.apply(get_transform());
         const point radius = (c + point(rx, ry)).apply(get_transform()) - c.apply(get_transform());

         QPainterPath path;
         path.addEllipse(convert(center), radius.x, radius.y);
         painter->fillPath(path, get_brush());
         return *this;
      }

      painter_drawing& painter_drawing::draw_oval(const point& c, double rx, double ry)
      {
         if (!painter)
            return *this;

         const point center = c.apply(get_transform());
         const point radius = (c + point(rx, ry)).apply(get_transform()) - c.apply(get_transform());

         painter->setPen(get_pen());
         painter->drawEllipse(convert(center), radius.x, radius.y);
         return *this;
      }

      painter_drawing& painter_drawing::fill_arc(const point& c, double rx, double ry, double angle1, double angle2)
      {
         if (!painter)
            return *this;

         const point center = c.apply(get_transform());
         const point top_left = (c - point(rx, ry)).apply(get_transform());
         const point bottom_right = (c + point(rx, ry)).apply(get_transform());
         const point w_h = bottom_right - top_left;
         const point mid_right = (c + point(rx, 0)).apply(get_transform());

         const double delta_angle = angle2 - angle1;

         QPainterPath path;
         path.moveTo(center.x, center.y);
         path.lineTo(mid_right.x, mid_right.y);
         path.arcTo(top_left.x, top_left.y, w_h.x, w_h.y, angle1 * 180 / PI, -delta_angle * 180 / PI);
         painter->fillPath(path, get_brush());
         return *this;
      }

      painter_drawing& painter_drawing::fill_rect(const rect& r)
      {
         if (!painter)
            return *this;

         point top_left = point(r.x, r.y).apply(get_transform());
         point bottom_right = point(r.x + r.width, r.y + r.height).apply(get_transform());

         double min_x = std::min(top_left.x, bottom_right.x);
         double max_x = std::max(top_left.x, bottom_right.x);
         double min_y = std::min(top_left.y, bottom_right.y);
         double max_y = std::max(top_left.y, bottom_right.y);

         painter->fillRect(min_x, min_y, max_x - min_x, max_y - min_y, get_brush());
         return *this;
      }

      painter_drawing& painter_drawing::draw_rect(const rect& r)
      {
         if (!painter)
            return *this;

         point top_left = point(r.x, r.y).apply(get_transform());
         point bottom_right = point(r.x + r.width, r.y + r.height).apply(get_transform());

         double min_x = std::min(top_left.x, bottom_right.x);
         double max_x = std::max(top_left.x, bottom_right.x);
         double min_y = std::min(top_left.y, bottom_right.y);
         double max_y = std::max(top_left.y, bottom_right.y);

         painter->setPen(get_pen());
         painter->drawRect(min_x, min_y, max_x - min_x, max_y - min_y);
         return *this;
      }

      rect painter_drawing::get_bounds() const
      {
         if (!painter)
            return rect(0, 0, 1, 1);

         const auto viewport = painter->viewport();
         return rect(viewport.x(), viewport.y(), viewport.width(), viewport.height());
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
