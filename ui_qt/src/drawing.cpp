#include <dak/ui_qt/drawing.h>
#include <dak/ui_qt/convert.h>

namespace dak
{
   namespace ui_qt
   {
      QPen drawing::get_pen() const
      {
         const auto& co = get_color();
         const auto& strk = get_stroke();
         return QPen(convert(co), strk.width, Qt::SolidLine, convert(strk.cap), convert(strk.join));
      }

      QBrush drawing::get_brush() const
      {
         const auto& co = get_color();
         return QBrush(convert(co));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
