#pragma once

#ifndef DAK_UI_STROKE_H
#define DAK_UI_STROKE_H

namespace dak
{
   namespace ui
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Stroke style to draw lines.

      class stroke
      {
      public:
         enum class cap_style
         {
            flat,
            square,
            round,
         };

         enum class join_style
         {
            miter,
            bevel,
            round,
         };

         // Width of the line.
         double width = 1.;

         // How lines are ended.
         cap_style cap = cap_style::round;

         // How lines are joined.
         join_style join = join_style::round;

         // Stroke with a width, line-end and line-join styles.
         stroke(double w, cap_style c, join_style j) : width(w), cap(c), join(j) { }

         // Stroke with a width.
         stroke(double w) : width(w) { }

         // Comparison.
         bool operator == (const stroke& other) const
         {
            return width == other.width && cap == other.cap && join == other.join;
         }

         bool operator != (const stroke& other) const
         {
            return !(*this == other);
         }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
