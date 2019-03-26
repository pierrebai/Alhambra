#include <dak/ui_qt/convert.h>

#include <dak/ui/drawing.h>

namespace dak
{
   namespace ui_qt
   {
      color convert(const QColor & qc)
      {
         return dak::ui::color(qc.red(), qc.green(), qc.blue(), qc.alpha());
      }

      QColor convert(const color& c)
      {
         return QColor(c.r, c.g, c.b, c.a);
      }

      point convert(const QPointF& qp)
      {
         return point(qp.x(), qp.y());
      }

      QPointF convert(const point& p)
      {
         return QPointF(p.x, p.y);
      }

      polygon convert(const QPolygonF& p)
      {
         polygon poly;
         for (const auto& pt : p)
            poly.points.emplace_back(convert(pt));
         return poly;
      }

      QPolygonF convert(const polygon& p)
      {
         QPolygonF poly;
         for (const point& pt : p.points)
            poly.append(convert(pt));
         return poly;
      }

      rect convert(const QRect& r)
      {
         return rect(convert(r.topLeft()), convert(r.bottomRight()));
      }

      QRectF convert(const rect & r)
      {
         return QRectF(convert(r.top_left()), convert(r.bottom_right()));
      }

      rect convert(const QSize& s)
      {
         return rect(point(0., 0.), point(s.width(), s.height()));
      }

      Qt::PenCapStyle convert(stroke::cap_style c)
      {
         switch (c)
         {
            case stroke::cap_style::flat:
               return Qt::PenCapStyle::FlatCap;
            case stroke::cap_style::square:
               return Qt::PenCapStyle::SquareCap;
            default:
            case stroke::cap_style::round:
               return Qt::PenCapStyle::RoundCap;
         }
      }

      Qt::PenJoinStyle convert(stroke::join_style j)
      {
         switch (j)
         {
            case stroke::join_style::miter:
               return Qt::PenJoinStyle::MiterJoin;
            case stroke::join_style::bevel:
               return Qt::PenJoinStyle::BevelJoin;
            default:
            case stroke::join_style::round:
               return Qt::PenJoinStyle::RoundJoin;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
