#pragma once

#ifndef DAK_UI_QT_TILING_CANVAS_H
#define DAK_UI_QT_TILING_CANVAS_H

#include <dak/ui_qt/canvas.h>

#include <dak/ui_qt/painter_transformable.h>

#include <dak/tiling/tiling.h>

#include <memory>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a tiling.

      class tiling_canvas : public canvas
      {
      public:
         // This is the tiling to draw on the canvas.
         tiling::tiling tiling;

         // Create a canvas with the given parent widget.
         tiling_canvas(QWidget* parent);

      protected:
         // This will draw the tiling, if any, then the transformer drawings, if any.
         void paint(QPainter& painter) override;

         // This will allow painting the tiling.
         dak::ui_qt::painter_transformable tiling_drawing;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
