#pragma once

#ifndef DAK_GEOMETRY_POLYGON_H
#define DAK_GEOMETRY_POLYGON_H

#include <dak/geometry/rect.h>

#include <vector>

namespace dak
{
   namespace geometry
   {
      class point;
      class transform;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A plain old polygon, in this case a dynamically-sized array of point
      // instances.

      class polygon
      {
      public:
         std::vector<point> points;

         // Empty polygon.
         polygon() { }

         // Polygon containing the given points.
         polygon(const std::vector<point>& pts) : points(pts) { }

         // Polygon containing a subset of the given points.
         polygon(const std::vector<point>& pts, int start, int end);

         // Make a regular polygon of n sides.
         static polygon make_regular(int n);

         // Make a polygon from a rectangle.
         static polygon from_rect(const rect& r) { return polygon(r.points()); }

         // Comparison.
         bool operator ==(const polygon& other) const
         {
            return points == other.points;
         }

         bool operator !=(const polygon& other) const
         {
            return !(points == other.points);
         }

         // Compare the points of the polygon.
         // The points are not re-ordered, so they may compare different even though they contain the same points.
         bool operator <(const polygon& other) const
         {
            return points < other.points;
         }

         // Apply a transform to the polygon.
         polygon apply(const transform& T) const;

         point center() const;

         // Perimeter of the polygon.
         double perimeter(bool closed = true) const;

         // Area of the polygon.
         double area() const;

         // The bounding rectangle containing the polygon.
         rect bounds() const;

         // Verify if a point is inside the polygon.
         bool is_inside(const point& apt) const;

         // Verify if the polygon is regular.
         bool is_regular() const;

         // Verify if the polygon intersect another polygon.
         bool intersects(const polygon& other) const;

         // Verify if the polygon is invalid. (Less than 3 sides.)
         bool is_invalid() const
         {
            return points.size() < 3;
         }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
