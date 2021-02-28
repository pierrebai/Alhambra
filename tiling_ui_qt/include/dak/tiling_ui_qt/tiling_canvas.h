#pragma once

#ifndef DAK_TILING_UI_QT_TILING_CANVAS_H
#define DAK_TILING_UI_QT_TILING_CANVAS_H

#include <dak/ui/qt/drawing_canvas.h>

#include <dak/tiling/tiling.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling::tiling_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a tiling.

      class tiling_canvas_t : public ui::qt::drawing_canvas_t
      {
      public:
         // This is the tiling to draw on the drawing_canvas.
         std::shared_ptr<tiling_t> tiling;

         // Create a drawing_canvas with the given parent widget.
         tiling_canvas_t(QWidget* parent);

      protected:
         // Paint using the prepared drawing.
         void draw(ui::drawing_t& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
