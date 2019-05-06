#pragma once

#ifndef DAK_UI_QT_MESSAGE_REPORTER_H
#define DAK_UI_QT_MESSAGE_REPORTER_H

#include <QtWidgets/qwidget.h>

#include <string>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Reporting information to the user over a given widget.
      //
      // TODO: maybe connect it to a generic info reporting system.

      class message_reporter
      {
      public:
         enum class category
         {
            info, warning, error
         };

         message_reporter(QWidget* on_widget);

         void report(const std::wstring& text, category cat);

         void paint(QPainter& painter);

      private:
         struct message
         {
            std::wstring text;
            category category;
            double opacity;
         };

         QWidget* widget = nullptr;
         std::vector<message> messages;
         bool has_repaint = false;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
