#include <dak/geometry/rect.h>
#include <dak/geometry/transform.h>

#include <algorithm>

namespace dak
{
   namespace geometry
   {
      rect rect::apply(const transform& trf) const
      {
         const point top_left = point(x, y).apply(trf);
         const point bottom_right = point(x + width, y + height).apply(trf);
         return rect(top_left, bottom_right);
      }

      rect rect::combine(const rect& other) const
      {
         const double min_x = std::min(x, other.x);
         const double min_y = std::min(y, other.y);
         const double max_x = std::max(x + width, other.x + other.width);
         const double max_y = std::max(y + height, other.y + other.height);

         return rect(min_x, min_y, max_x - min_x, max_y - min_y);
      }

      rect rect::combine(const point& pt) const
      {
         const double min_x = std::min(x, pt.x);
         const double min_y = std::min(y, pt.y);
         const double max_x = std::max(x + width, pt.x);
         const double max_y = std::max(y + height, pt.y);

         return rect(min_x, min_y, max_x - min_x, max_y - min_y);
      }

      rect rect::intersect(const rect& other) const
      {
         const double min_x = std::max(x, other.x);
         const double min_y = std::max(y, other.y);
         const double max_x = std::min(x + width, other.x + other.width);
         const double max_y = std::min(y + height, other.y + other.height);

         if (max_x < min_x || max_y < min_y) {
            return rect();
         }
         else {
            return rect(min_x, min_y, max_x - min_x, max_y - min_y);
         }
      }

      rect rect::central_scale(double s) const
      {
         double cx = x + width * 0.5;
         double cy = y + height * 0.5;

         double nw = width * s;
         double nh = height * s;

         return rect(cx - nw * 0.5, cy - nh * 0.5, nw, nh);
      }

      transform rect::center_inside(const rect& other)
      {
         double xscale = other.width / width;
         double yscale = other.height / height;

         double scale = std::min(xscale, yscale);

         transform Ts = transform::scale(scale, scale);

         const point my_center = point(x + (width / 2.0), y + (height / 2.0));
         const point your_center = point(other.x + (other.width / 2.0), other.y + (other.height / 2.0));

         return transform::translate(your_center).compose(Ts).compose(
            transform::translate(my_center.scale(-1.0)));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
