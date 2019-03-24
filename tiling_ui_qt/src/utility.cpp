#include <dak/tiling_ui_qt/utility.h>

#include <dak/ui_qt/mosaic_canvas.h>
#include <dak/ui_qt/painter_drawing.h>

#include <dak/tiling_style/colored.h>

#include <QtGui/qpixmap.h>
#include <QtGui/qpainter.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using ui_qt::mosaic_canvas;
      using ui_qt::painter_drawing;
      using tiling_style::colored;

      QIcon get_icon(const std::shared_ptr<styled_mosaic>& sm, int w, int h)
      {
         if (!sm)
            return QIcon();

         color co = color::black();
         if (auto style = std::dynamic_pointer_cast<colored>(sm->style))
             co = style->color;
         return get_icon(sm->mosaic, co, w, h);
      }

      QIcon get_icon(const std::shared_ptr<mosaic>& mosaic, const color& co, int w, int h)
      {
         if (!mosaic)
            return QIcon();

         QPixmap pixmap(w, h);
         QPainter painter(&pixmap);
         painter_drawing drw(painter);
         draw_tiling(drw, mosaic, co, 1);

         return QIcon(pixmap);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
