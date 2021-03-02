#include <dak/tiling/inflation_tiling.h>

#include <dak/geometry/intersect.h>

namespace dak
{
   namespace tiling
   {
      inflation_tiling_t::inflation_tiling_t()
      {
      }

      inflation_tiling_t::inflation_tiling_t(const std::wstring& name, const edge_t& s1, const edge_t& s2, double factor)
         : tiling_t(name), s1(s1), s2(s2), factor(factor)
      {
      }

      void inflation_tiling_t::swap(inflation_tiling_t& other)
      {
         s1.swap(other.s1);
         s2.swap(other.s2);
         std::swap(factor, other.factor);
         return tiling_t::swap(other);
      }

      bool inflation_tiling_t::is_invalid() const
      {
         return tiling_t::is_invalid() || s1.is_invalid() || s1.is_trivial() || s2.is_invalid() || s2.is_trivial() || factor <= 1.;
      }

      bool inflation_tiling_t::operator==(const tiling_t& other) const
      {
         if (!tiling_t::operator==(other))
            return false;

         if (auto other_tt = dynamic_cast<const inflation_tiling_t*>(&other))
            return s1 == other_tt->s1 && s2 == other_tt->s2 && dak::utility::near(factor, other_tt->factor);

         return false;
      }

      void inflation_tiling_t::fill(const rectangle_t& region, std::function<void(const tiling_t& tiling, const transform_t&)> fill_callback) const
      {
         // Get smallest angle betwen the edges. (That is why we limit it to be below PI.)
         double angle = s1.angle(s2);
         if (angle > geometry::PI)
            angle -= geometry::PI;

         // Edge too close together would be degenerate and cause too many copy, reject.
         if (angle <= geometry::PI / 100)
            return;


         // Calculate how many copies around a circle we need.
         //
         // While it seems impossible to have less than one since we limited
         // the angle to be between PI/100 and PI, so this should never be less than 1,
         // the vagaries of code changes being what they are, it costs little to verify.
         const int circle_fraction = int(std::ceil(2 * geometry::PI / angle));
         if (circle_fraction < 1)
            return;

         // Find where the edges meet.
         //
         // Again, due to the checks above, this should succeed, but better safe than sorry.
         const point_t center = geometry::intersect::intersect_anywhere(s1.p1, s1.p2, s2.p1, s2.p2);
         if (center.is_invalid())
            return;

         double total_factor = 1.;
         for (int s = 0; s < 8; ++s)
         {
            transform_t inflation = transform_t::scale(center, total_factor);
            double total_angle = 0.;
            for (int i = 0; i < circle_fraction; ++i, total_angle += angle)
            {
               transform_t rotation = transform_t::rotate(center, total_angle);
               fill_callback(*this, inflation.compose(rotation));
            }
            
            total_factor *= factor;
         }
      }

      size_t inflation_tiling_t::count_fill_copies(const rectangle_t& region) const
      {
         // TODO
         return 1;
      }

      void inflation_tiling_t::surround(std::function<void(const tiling_t& tiling, const transform_t&)> fill_callback) const
      {
         // TODO: common code with fill?

         // Get smallest angle betwen the edges. (That is why we limit it to be below PI.)
         double angle = s1.angle(s2);
         if (angle > geometry::PI)
            angle -= geometry::PI;

         // Edge too close together would be degenerate and cause too many copy, reject.
         if (angle <= geometry::PI / 100)
            return;


         // Calculate how many copies around a circle we need.
         //
         // While it seems impossible to have less than one since we limited
         // the angle to be between PI/100 and PI, so this should never be less than 1,
         // the vagaries of code changes being what they are, it costs little to verify.
         const int circle_fraction = int(std::ceil(2 * geometry::PI / angle));
         if (circle_fraction < 1)
            return;

         // Find where the edges meet.
         //
         // Again, due to the checks above, this should succeed, but better safe than sorry.
         const point_t center = geometry::intersect::intersect_anywhere(s1.p1, s1.p2, s2.p1, s2.p2);
         if (center.is_invalid())
            return;

         double total_factor = 1.;
         for (int s = 0; s < 3; ++s)
         {
            transform_t inflation = transform_t::scale(center, total_factor);
            double total_angle = 0.;
            for (int i = 0; i < circle_fraction; ++i, total_angle += angle)
            {
               transform_t rotation = transform_t::rotate(center, total_angle);
               fill_callback(*this, inflation.compose(rotation));
            }

            total_factor *= factor;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
