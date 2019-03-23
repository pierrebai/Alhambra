#pragma once

#ifndef DAK_UI_QT_PAINTER_TRANSFORMABLE_H
#define DAK_UI_QT_PAINTER_TRANSFORMABLE_H

#include <dak/ui_qt/painter_drawing.h>

#include <dak/ui/transformable.h>

namespace dak
{
   namespace ui_qt
   {
      using geometry::point;
      using geometry::transform;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Enable editing the transform of a QPainter.

      class painter_transformable : public painter_drawing, public ui::transformable
      {
      public:
         // Empty transformable.
         painter_transformable() : painter_drawing() { }

         // Transformable for the given QPainter.
         painter_transformable(QPainter& p) : painter_drawing(p) { }

         // ui::transformable implementation.
         const transform& get_transform() const override;
         painter_transformable& set_transform(const transform& t) override;
         painter_transformable& compose(const transform& t) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
