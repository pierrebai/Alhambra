#pragma once

#ifndef DAK_UI_QT_DRAWING_H
#define DAK_UI_QT_DRAWING_H

#include <dak/ui/drawing_base.h>

#include <dak/ui/layered.h>

#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

namespace dak
{
   namespace tiling
   {
      class mosaic;
      class tiling;
   }

   namespace ui_qt
   {
      using tiling::mosaic;
      using ui::color;
      using ui::drawing;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Drawing that can translate color, stroke, etc to QT format.

      class qt_drawing : public ui::drawing_base
      {
      protected:
         QPen get_pen() const;
         QBrush get_brush() const;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Draw the tiling or mosaic, if any, in the given drawing.

      void draw_tiling(drawing& drw, const tiling::tiling& tiling, const color& co, int copy_count);
      void draw_tiling(drawing& drw, const std::shared_ptr<tiling::mosaic>& mosaic, const color& co, int copy_count);

      void draw_layered(drawing& drw, ui::layered* layered);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
