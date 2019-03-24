#pragma once

#ifndef DAK_UI_QT_DRAWING_H
#define DAK_UI_QT_DRAWING_H

#include <dak/ui/drawing_base.h>

#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

namespace dak
{
   namespace tiling
   {
      class mosaic;
   }

   namespace ui_qt
   {
      using tiling::mosaic;
      using ui::color;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Drawing that can translate color, stroke, etc to QT format.

      class drawing : public ui::drawing_base
      {
      protected:
         QPen get_pen() const;
         QBrush get_brush() const;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Draw the mosaic, if any, in the given drawing.

      void draw_tiling(dak::ui::drawing& drw, const std::shared_ptr<tiling::mosaic>& mosaic, const color& co, int copy_count);

   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
