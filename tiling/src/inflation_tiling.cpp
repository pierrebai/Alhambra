#include <dak/tiling/inflation_tiling.h>

#include <dak/geometry/intersect.h>

namespace dak
{
   namespace tiling
   {
      inflation_tiling_t::inflation_tiling_t()
      {
      }

      inflation_tiling_t::inflation_tiling_t(const std::wstring& name, const edge_t& a_s1, const edge_t& a_s2, const transform_t& an_inflation)
         : tiling_t(name), s1(a_s1), s2(a_s2), inflation(an_inflation)
      {
      }

      void inflation_tiling_t::swap(inflation_tiling_t& other)
      {
         s1.swap(other.s1);
         s2.swap(other.s2);
         std::swap(inflation, other.inflation);
         return tiling_t::swap(other);
      }

      bool inflation_tiling_t::is_invalid() const
      {
         return tiling_t::is_invalid() || s1.is_invalid() || s1.is_trivial() || s2.is_invalid() || s2.is_trivial() || inflation.is_invalid();
      }

      bool inflation_tiling_t::operator==(const tiling_t& other) const
      {
         if (!tiling_t::operator==(other))
            return false;

         if (auto other_tt = dynamic_cast<const inflation_tiling_t*>(&other))
            return s1 == other_tt->s1 && s2 == other_tt->s2 && inflation == other_tt->inflation;

         return false;
      }

      point_t inflation_tiling_t::get_center() const
      {
         const point_t center = geometry::intersect::intersect_anywhere(s1.p1, s1.p2, s2.p1, s2.p2);
         return center;
      }

      void inflation_tiling_t::fill_rings(int rings_count, std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const
      {
         // Get smallest angle betwen the edges. (That is why we limit it to be below PI.)
         double angle = s1.angle(s2);
         if (angle > geometry::PI)
            angle = 2 * geometry::PI - angle;

         // Edge too close together would be degenerate and cause too many copy, reject.
         if (angle <= geometry::PI / 100)
            return;

         // Calculate how many copies around a circle we need.
         //
         // While it seems impossible to have less than one since we limited
         // the angle to be between PI/100 and PI, so this should never be less than 1,
         // the vagaries of code changes being what they are, it costs little to verify.
         const int circle_fraction = int(std::round(2 * geometry::PI / angle));
         if (circle_fraction < 1)
            return;

         // Find where the edges meet.
         //
         // Again, due to the checks above, this should succeed, but better safe than sorry.
         const point_t center = get_center();
         if (center.is_invalid())
            return;

         transform_t total_inflation = transform_t::identity();
         for (int s = 0; s < rings_count; ++s)
         {
            double total_angle = 0.;
            for (int i = 0; i < circle_fraction; ++i, total_angle += angle)
            {
               transform_t rotation = transform_t::rotate(center, total_angle);
               fill_callback(*this, rotation.compose(total_inflation));
            }

            total_inflation = total_inflation.compose(inflation);
         }
      }

      void inflation_tiling_t::fill(const rectangle_t& region, std::function<void(const tiling_t& tiling, const transform_t&)> fill_callback) const
      {
         fill_rings(8, fill_callback);
      }

      size_t inflation_tiling_t::count_fill_copies(const rectangle_t& region) const
      {
         // TODO: real number of inflation copies needed to fill a region.
         return 32;
      }

      void inflation_tiling_t::surround(std::function<void(const tiling_t& tiling, const transform_t&)> fill_callback) const
      {
         fill_rings(3, fill_callback);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
