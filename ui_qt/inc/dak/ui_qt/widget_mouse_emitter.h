#pragma once

#ifndef DAK_UI_QT_WIDGET_MOUSE_EMITTER_H
#define DAK_UI_QT_WIDGET_MOUSE_EMITTER_H

#include <dak/ui/mouse.h>

#include <QtWidgets/qwidget.h>

namespace dak
{
   namespace ui_qt
   {
      using geometry::point;
      using ui::modifiers;

      namespace mouse
      {
         using ui::mouse::buttons;

         ////////////////////////////////////////////////////////////////////////////
         //
         // Generator of mouse events for a QWidget.

         class widget_emitter : public ui::mouse::emitter, public QObject
         {
         public:
            // Widget that will give the events
            QWidget& widget;

            // Create the emitter on the given widget.
            // An event filter will be placed on the widget.
            widget_emitter(QWidget& w);

            // Destructor.
            // The event filter will be removed from the widget.
            ~widget_emitter();

         protected:
            bool eventFilter(QObject* obj, QEvent* ev) override;
         };

      }
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
