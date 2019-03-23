#pragma once

#ifndef DAK_UI_EPS_DRAWING_H
#define DAK_UI_EPS_DRAWING_H

#include <dak/ui/drawing_base.h>

#include <ostream>

namespace dak
{
   namespace ui
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Drawing into an encapsulated postscript (EPS).

      class eps_drawing : public drawing_base
      {
      public:
         eps_drawing(std::wostream& out);

         ~eps_drawing() override;

         // drawing interface implementation.
         drawing& draw_line(const point& from, const point& to) override;
         drawing& fill_polygon(const polygon& p) override;
         drawing& draw_polygon(const polygon& p) override;
         drawing& fill_oval(const point& c, double rx, double ry) override;
         drawing& draw_oval(const point& c, double rx, double ry) override;
         drawing& fill_arc(const point& c, double rx, double ry, double angle1, double angle2) override;
         drawing& fill_rect(const rect& r) override;
         drawing& draw_rect(const rect& r) override;

         // Note: the bounds are *without* the transform.
         //       They are the true bounds of the drawing surface.
         rect get_bounds() const override;

      private:
         void internal_update_color();
         void internal_update_stroke();
         void internal_update_transform();

         void internal_update_bbox(const std::vector<point>& pts);
         void internal_update_bbox(const point& p);

         std::wostream& out;

         color applied_co = color::black();
         stroke applied_strk = stroke(1);
         transform applied_trf = transform::identity();
         rect bbox;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
