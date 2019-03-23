#include <dak/geometry/transform.h>

namespace dak
{
   namespace geometry
   {
      const transform transform::invalid;

      transform transform::rotate(double t)
      {
         return transform(
            std::cos(t), -std::sin(t), 0,
            std::sin(t), std::cos(t), 0);
      }

      transform transform::rotate(const point& pt, double t)
      {
         return translate(pt).compose(
            rotate(t).compose(
               translate(-pt.x, -pt.y)));
      }

      double transform::dist_from_zero(double v) const
      {
         return point(v, 0.0).apply(*this).distance(point::origin().apply(*this));
      }

      double transform::dist_from_inverted_zero(double v) const
      {
         return invert().dist_from_zero(v);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
