#pragma once

#ifndef DAK_UI_QT_TRANSFORMABLE_CANVAS_H
#define DAK_UI_QT_TRANSFORMABLE_CANVAS_H

#include <dak/ui_qt/widget_mouse_emitter.h>

#include <dak/ui/transformer.h>

#include <QtWidgets/qwidget>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget that brings together a transformer and widget mouse emitter
      // with some transformable object to be provided by a sub-class.

      class canvas : public QWidget
      {
      public:
         // This will allow changing the transform of the drawing.
         dak::ui::transformer transformer;

         // Create a canvas with the given parent widget and transformable object.
         canvas(QWidget* parent);
         canvas(QWidget* parent, ui::transformable& trfable);

      protected:
         // This will draw the transformer drawings, if any.
         void paintEvent(QPaintEvent * pe) override;

         // This gets called by the paint event to do the painting.
         // Sub-class should override this and call this implementation last.
         virtual void paint(QPainter& painter);

         // This will allow receiving mouse event from the canvas.
         dak::ui_qt::mouse::widget_emitter emi;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
