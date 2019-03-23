#pragma once

#ifndef DAK_GEOMETRY_EDGE_H
#define DAK_GEOMETRY_EDGE_H

#include <dak/geometry/point.h>
#include <dak/geometry/constants.h>

namespace dak
{
   namespace geometry
   {
      class transform;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Oriented edge connecting two points in a geometrical map.

      class edge
      {
      private:
         static constexpr double very_negative = -1e100;

      public:

         // Three common edges.
         static edge unit_x() { return edge(point::origin(), point::unit_x()); }
         static edge unit_y() { return edge(point::origin(), point::unit_y()); }
         static const edge invalid;

         // The ordered points.
         point p1;
         point p2;

         // The order of edges around a point. Roughly anti-clockwise angle.
         double order = very_negative;

         // Invalid edge.
         constexpr edge() { }

         // Edge between two points.
         edge(const point& p1, const point& p2) : p1(p1), p2(p2), order(angle()) { }

         // Special edge used to search for the first edge around a point, when they are sorted.
         static edge lowest_edge(const point& p1)
         {
            return edge(p1, very_negative);
         }

         // Special edge used to search for the last edge around a point, when they are sorted.
         static edge highest_edge(const point& p1)
         {
            return edge(p1, -very_negative);
         }

         // Swap two edges.
         constexpr void swap(edge& other)
         {
            p1.swap(other.p1);
            p2.swap(other.p2);
            { const double t = order; order = other.order; other.order = t; }
         }

         // Edge comparisons. Two invalid are equal.
         bool operator ==(const edge& other) const
         {
            return p1 == other.p1 && p2 == other.p2;
         }

         bool operator !=(const edge& other) const
         {
            return !(*this == other);
         }

         // The less-than operator compares the first point, then the order.
         // Thus, two overlapped edges with different second point will not be less than each other,
         // yet they won't be equal. This cannot happen in a map anyway since edges never overlap.
         bool operator <(const edge& other) const
         {
            return (p1 < other.p1)
                || (p1 == other.p1 && order < other.order - TOLERANCE);
         }

         // Possibly swap the points of the edge to make the first point be less than the second.
         edge& make_canonical()
         {
            if (p2 < p1)
            {
               p1.swap(p2);
               order = angle();
            }
            return *this;
         }

         // Create a copy that is guaranteed to be canonical if the original was a valid edge.
         edge canonical() const
         {
            return p1 < p2 ? edge(p1, p2) : edge(p2, p1);
         }

         // Create a copy that has the points swapped.
         edge twin() const
         {
            return edge(p2, p1);
         }

         // Verify of the edge is canonical.
         bool is_canonical() const
         {
            return p1 < p2;
         }

         // Verify of the edge is invalid.
         constexpr bool is_invalid() const
         {
            return order == very_negative;
         }

         // Verify of the edge is trivial (both points are equal).
         constexpr bool is_trivial() const
         {
            return p1 == p2;
         }

         // Calculates the angle between two edges.
         double angle(const edge& other) const
         {
            double res = other.angle() - angle();

            while (res < 0.0)
               res += 2.0 * PI;

            return res;
         }

         // Calculates the angle the edges and the x-axis.
         double angle() const
         {
            return p1.angle(p2);
         }

         // Apply a transform to an edge.
         edge apply(const transform& t) const
         {
            return edge(p1.apply(t), p2.apply(t));
         }

      private:
         constexpr edge(const point& p1, double order) : p1(p1), p2(), order(order) { }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
