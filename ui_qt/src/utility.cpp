#include <dak/ui_qt/utility.h>

#include <dak/ui_qt/mosaic_canvas.h>
#include <dak/ui_qt/painter_drawing.h>

#include <dak/ui/color.h>

#include <dak/tiling_style/colored.h>

#include <QtGui/qpainter.h>

#include <QtWinExtras/qwinfunctions.h>

namespace dak
{
   namespace ui_qt
   {
      using ui_qt::mosaic_canvas;
      using ui_qt::painter_drawing;
      using namespace tiling;
      using namespace tiling_style;
      using namespace dak::ui;

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

      QPixmap create_pixmap_from_resource(int res)
      {
         QPixmap pixmap = QtWin::fromHBITMAP((HBITMAP)::LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(res)), QtWin::HBitmapNoAlpha);
         pixmap.setMask(pixmap.createMaskFromColor(QColor(255, 255, 255, 255)));
         return pixmap;
      }

      QToolButton* create_tool_button()
      {
         QToolButton* button = new QToolButton;
         button->setIconSize(QSize(64, 64));
         button->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextUnderIcon);
         return button;
      }

      QToolButton* create_tool_button(const wchar_t* text, int icon, QKeySequence key, const wchar_t* tooltip, std::function<void()> callback)
      {
         return create_tool_button(create_action(text, icon, key, tooltip, callback));
      }

      QToolButton* create_tool_button(QAction* action)
      {
         QToolButton* button = create_tool_button();
         button->setDefaultAction(action);
         return button;
      }

      QAction* create_action(const wchar_t* text, int icon, QKeySequence key, const wchar_t* tooltip, std::function<void()> callback)
      {
         QAction* action = new QAction;
         if (text && text[0])
            action->setText(QString::fromWCharArray(text));
         if (icon > 0)
            action->setIcon(QIcon(create_pixmap_from_resource(icon)));
         if (!key.isEmpty())
            action->setShortcut(key);
         if (tooltip && tooltip[0])
            action->setToolTip(QString::fromWCharArray(tooltip));
         if (callback)
            action->connect(action, &QAction::triggered, callback);
         return action;
      }

   }
}

// vim: sw=3 : sts=3 : et : sta : 
