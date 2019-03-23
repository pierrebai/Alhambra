#pragma once

#ifndef DAK_UI_QT_DRAWING_H
#define DAK_UI_QT_DRAWING_H

#include <dak/ui/drawing_base.h>

#include <QtGui/qpen.h>
#include <QtGui/qbrush.h>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Drawing that can translate color, stroke, etc to QT format.

      class drawing : public ui::drawing_base
      {
      protected:
         QPen get_pen() const;
         QBrush get_brush() const;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
