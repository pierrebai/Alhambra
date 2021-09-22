#pragma once

#ifndef DAK_TILING_UI_QT_MOSAIC_CANVAS_H
#define DAK_TILING_UI_QT_MOSAIC_CANVAS_H

#include <dak/ui/qt/drawing_canvas.h>

#include <dak/tiling/mosaic.h>

#include <memory>

namespace dak
{
   namespace tiling_ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A drawing canvas widget (knows how to paint and move the painting using
      // the mouse) that draws a mosaic.

      class mosaic_canvas_t : public ui::qt::drawing_canvas_t
      {
      public:
         // This is the mosaic to draw on the drawing_canvas.
         std::shared_ptr<dak::tiling::mosaic_t> mosaic;

         // Create a drawing_canvas with the given parent widget.
         mosaic_canvas_t(QWidget* parent);

      protected:
         // Paint the mosaic using the prepared drawing.
         void draw(ui::drawing_t& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
