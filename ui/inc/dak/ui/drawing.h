#pragma once

#ifndef DAK_UI_DRAWING_H
#define DAK_UI_DRAWING_H

#include <dak/geometry/rect.h>
#include <dak/geometry/point.h>
#include <dak/geometry/polygon.h>
#include <dak/geometry/transform.h>

#include <dak/ui/color.h>
#include <dak/ui/stroke.h>

namespace dak
{
   namespace ui
   {
      using geometry::point;
      using geometry::polygon;
      using geometry::transform;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // API needed to draw.

      class drawing
      {
      public:
         virtual ~drawing() = 0 { }

         // Stroke manipulations.
         virtual stroke get_stroke() const = 0;
         virtual drawing& set_stroke(const stroke& s) = 0;

         // Color manipulations.
         virtual color get_color() const = 0;
         virtual drawing& set_color(const color& c) = 0;

         // Drawing.
         virtual drawing& draw_line(const point& from, const point& to) = 0;
         virtual drawing& draw_corner(const point& from, const point& corner, const point& to) = 0;
         virtual drawing& fill_polygon(const polygon& p) = 0;
         virtual drawing& draw_polygon(const polygon& p) = 0;
         virtual drawing& fill_oval(const point& c, double rx, double ry) = 0;
         virtual drawing& draw_oval(const point& c, double rx, double ry) = 0;
         virtual drawing& fill_arc(const point& c, double rx, double ry, double angle1, double angle2) = 0;
         virtual drawing& fill_rect(const rect& r) = 0;
         virtual drawing& draw_rect(const rect& r) = 0;

         void draw_arrow(const point& from, const point& to, double length, double half_width);
         void fill_arrow(const point& from, const point& to, double length, double half_width);

         // Note: the bounds are *without* the transform.
         //       They are the true bounds of the drawing surface.
         virtual rect get_bounds() const = 0;

         // Transform applied to all the points in the drawing commands.
         virtual const transform& get_transform() const = 0;
         virtual drawing& set_transform(const transform&) = 0;
         virtual drawing& compose(const transform&) = 0;
         virtual drawing& push_transform() = 0;
         virtual drawing& pop_transform() = 0;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
