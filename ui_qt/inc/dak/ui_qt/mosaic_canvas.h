#pragma once

#ifndef DAK_UI_QT_MOSAIC_CANVAS_H
#define DAK_UI_QT_MOSAIC_CANVAS_H

#include <dak/ui_qt/canvas.h>

#include <dak/ui_qt/painter_transformable.h>

#include <dak/tiling/mosaic.h>

#include <memory>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a mosaic.

      class mosaic_canvas : public canvas
      {
      public:
         // This is the mosaic to draw on the canvas.
         std::shared_ptr<tiling::mosaic> mosaic;

         // Create a canvas with the given parent widget.
         mosaic_canvas(QWidget* parent);

         // Draw the mosaic, if any, in the given drawing.
         void draw_tiling(dak::ui::drawing& drw) const;

      protected:
         // This will draw the mosaic, if any, then the transformer drawings, if any.
         void paint(QPainter& painter) override;

         // This will allow painting the mosaic.
         dak::ui_qt::painter_transformable mosaic_drawing;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
