#include <dak/geometry/point.h>
#include <dak/geometry/constants.h>
#include <dak/geometry/transform.h>

namespace dak
{
   namespace geometry
   {
      const point point::invalid;

      double point::mag() const
      {
         return std::sqrt(mag_2());
      }

      double point::distance(const point& other) const
      {
         return std::sqrt(distance_2(other));
      }

      point point::normalize() const
      {
         double m = mag();
         if (m != 0.0)
            return scale(1.0 / m);
         else
            return *this;
      }

      point point::convex_sum(const point& other, double t) const
      {
         double mt = 1.0 - t;
         return point(mt * x + t * other.x, mt * y + t * other.y);
      }

      // Return the absolute angle of the edge from this to other, in the
      // range -PI to PI.
      double point::angle(const point& other) const
      {
         return std::atan2(other.y - y, other.x - x);
      }

      // Angle wrt the origin.
      double point::angle() const
      {
         return std::atan2(y, x);
      }

      // Get the section of arc swept out between the edges this ==> from
      // and this ==> to.
      double point::sweep(const point& from, const point& to) const
      {
         double res = angle(to) - angle(from);

         while (res < 0.0)
            res += 2.0 * PI;

         return res;
      }

      double point::distance_to_line(const point& p, const point& q) const
      {
         return std::sqrt(distance_2_to_line(p, q));
      }

      double point::distance_2_to_line(point p, const point& q) const
      {
         const point qmp = q - p;
         const double t = (*this - p).dot(qmp) / qmp.dot(qmp);
         if (t >= 0.0 && t <= 1.0)
         {
            double ox = p.x + t * (q.x - p.x);
            double oy = p.y + t * (q.y - p.y);
            return (x - ox)*(x - ox) + (y - oy)*(y - oy);
         }
         else if (t < 0.0)
         {
            return distance_2(p);
         }
         else
         {
            return distance_2(q);
         }
      }

      double point::parameterization_on_line(const point& p, const point& q) const
      {
         point qmp = q - p;
         return (*this - p).dot(qmp) / qmp.dot(qmp);
      }

      point point::apply(const transform& t) const
      {
         return point(
            t.scale_x * x + t.rot_1 * y   + t.trans_x,
            t.rot_2 * x   + t.scale_y * y + t.trans_y);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

