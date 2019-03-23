#pragma once

#ifndef DAK_GEOMETRY_RECT_H
#define DAK_GEOMETRY_RECT_H

#include <dak/geometry/point.h>
#include <dak/geometry/transform.h>

#include <vector>
#include <cmath>

namespace dak
{
   namespace geometry
   {
      class transform;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A Rectangle based on const point&.

      class rect
      {
      public:
         double x;
         double y;

         double width;
         double height;

         // Invalid rectangle.
         constexpr rect()
         : x(NAN), y(NAN), width(NAN), height(NAN)
         {
         }

         // Rectangle starting at the given coordinate with the given size.
         constexpr rect(double x, double y, double width, double height)
         : x(x), y(y), width(width), height(height)
         {
         }

         // Rectangle starting at the given point with the given size.
         constexpr rect(const point& pt, double width, double height)
            : x(pt.x), y(pt.y), width(width), height(height)
         {
         }

         // Rectangle starting at the given top-left point down to the given bottom-right point.
         constexpr rect(const point& tl, const point& br)
            : x(tl.x), y(tl.y), width(br.x - tl.x), height(br.y - tl.y)
         {
         }

         // The corners of the rectangle.
         point top_left() const { return point(x, y); }
         point top_right() const { return point(x+width, y); }
         point bottom_left() const { return point(x, y+height); }
         point bottom_right() const { return point(x+width, y+height); }

         // All points of the rectangle.
         std::vector<point> points() const { return std::vector<point>({ top_left(), top_right(), bottom_right(), bottom_left() }); }

         // Verify if invalid.
         constexpr bool is_invalid() const
         {
            const double n = NAN;
            return reinterpret_cast<const uint64_t&>(n) == reinterpret_cast<const uint64_t&>(x);
         }

         // Comparison. Two invalid are equal.
         constexpr bool operator ==(const rect& other) const
         {
            if (is_invalid())
               return other.is_invalid();

            return x == other.x && y == other.y && width == other.width && height == other.height;
         }

         constexpr bool operator !=(const rect& other) const
         {
            return !(*this == other);
         }

         // Apply a transform to the rect.
         // The rect will stay orthogonal though. Its two corners will have moved.
         rect apply(const transform& t) const;

         // Combine two rectangles.
         rect combine(const rect& other) const;

         // Combine a rectangle with a point.
         // Make sure the rectangle is big enough to contain the point.
         rect combine(const point& pt) const;

         // Intersect two rectangles.
         rect intersect(const rect& other) const;

         // Scale a rectangle from its center.
         rect central_scale(double s) const;

         // Create a transform that will place this rectangle inside the other.
         transform center_inside(const rect& other);

         // Return the rectangle center.
         point center() const { return point(x, y) + point(width,height).scale(0.5); }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
