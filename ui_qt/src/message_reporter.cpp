
#include <dak/ui_qt/message_reporter.h>

#include <QtCore/qtimer.h>

#include <QtGui/qpainter.h>
#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

#include <QtWidgets/qwidget.h>

#include <algorithm>

namespace dak
{
   namespace ui_qt
   {
      namespace
      {
         const std::map<message_reporter::category, QColor> category_colors =
         {
            { message_reporter::category::info,    QColor(140, 240, 140, 255) },
            { message_reporter::category::warning, QColor(200, 200, 140, 255) },
            { message_reporter::category::error,   QColor(200, 140, 140, 255) },
         };
      }

      message_reporter::message_reporter(QWidget* on_widget)
      : widget(on_widget)
      {
      }

      void message_reporter::report(const std::wstring& text, category cat)
      {
         messages.emplace(messages.begin(), message{ text, cat, 1.9 });
         widget->repaint();
      }

      void message_reporter::paint(QPainter& painter)
      {
         if (messages.size() <= 0)
            return;

         auto avail_bounds = widget->rect();
         avail_bounds.adjust(4, 4, -4, -4);
         for (auto& msg : messages)
         {
            auto font = painter.font();
            font.setPointSize(10);
            painter.setFont(font);

            const auto text = QString::fromStdWString(msg.text);
            const auto options = QTextOption(Qt::AlignTop | Qt::AlignLeft);

            auto bounds = painter.boundingRect(avail_bounds, text, options);
            bounds.adjust(-2, -2, 2, 2);

            const int alpha = std::min(240, int(msg.opacity * 240));
            auto color = category_colors.at(msg.category);
            color.setAlpha(alpha);
            painter.fillRect(bounds, QBrush(color));

            painter.setPen(QColor(0, 0, 0, alpha));
            painter.drawText(bounds, text, options);

            avail_bounds.setY(avail_bounds.y() + bounds.height() + 2);

            msg.opacity -= 0.01;
         }

         messages.erase(std::remove_if(messages.begin(), messages.end(), [](const message& msg) { return msg.opacity <= 0; } ), messages.end());

         if (!has_repaint)
         {
            has_repaint = true;
            QTimer::singleShot(20, [&has_repaint=has_repaint, &widget=widget]() { has_repaint = false; widget->repaint(); });
         }
      }

   }
}

// vim: sw=3 : sts=3 : et : sta : 
