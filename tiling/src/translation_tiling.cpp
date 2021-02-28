#include <dak/tiling/tiling.h>
#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling
   {
      using geometry::point_t;
      using geometry::polygon_t;
      using geometry::transform_t;

      translation_tiling_t::translation_tiling_t()
      {
      }

      translation_tiling_t::translation_tiling_t(const std::wstring& name, const point_t& t1, const point_t& t2)
         : tiling_t(name), t1(t1), t2(t2)
      {
      }

      void translation_tiling_t::swap(translation_tiling_t& other)
      {
         t1.swap(other.t1);
         t2.swap(other.t2);
         return tiling_t::swap(other);
      }

      bool translation_tiling_t::is_invalid() const
      {
         return tiling_t::is_invalid() || t1.is_invalid() || t1 == point_t(0., 0.) || t2.is_invalid() || t2 == point_t(0., 0.);
      }

      bool translation_tiling_t::operator==(const tiling_t& other) const
      {
         if (!tiling_t::operator==(other))
            return false;

         if (auto other_tt = dynamic_cast<const translation_tiling_t*>(&other))
            return t1 == other_tt->t1 && t2 == other_tt->t2;

         return false;
      }

      void translation_tiling_t::fill(const rectangle_t& region, std::function<void(const tiling_t& tiling, const transform_t&)> fill_callback) const
      {
         geometry::fill(region, t1, t2, [self=this, fill_callback](int t1, int t2) {
            const transform_t placement = transform_t::translate(self->t1.scale(t1) + self->t2.scale(t2));
            fill_callback(*self, placement);
         });
      }

      void translation_tiling_t::surround(std::function<void(const tiling_t& tiling, const transform_t&)> fill_callback) const
      {
         // I'm going to generate all the tiles in the translational units
         // (x,y) where -1 <= x, y <= 1.  This is guaranteed to surround
         // every tile in the (0,0) unit by tiles, unless the base unit
         // is very degenerate with very distant tiles that don't touch
         // each other.
         for (int y = -1; y <= 1; ++y)
            for (int x = -1; x <= 1; ++x)
               fill_callback(*this, transform_t::translate(t1.scale(x) + t2.scale(y)));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
