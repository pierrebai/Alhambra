#include <dak/tiling/tiling.h>

namespace dak
{
   namespace tiling
   {
      using geometry::point_t;
      using geometry::polygon_t;
      using geometry::transform_t;

      tiling_t::tiling_t()
      {
      }

      tiling_t::tiling_t(const std::wstring& name, const point_t& t1, const point_t& t2) : t1(t1), t2(t2), name(name)
      {
      }

      void tiling_t::swap(tiling_t& other)
      {
         t1.swap(other.t1);
         t2.swap(other.t2);
         tiles.swap(other.tiles);
         name.swap(other.name);
         description.swap(other.description);
         author.swap(other.author);
      }

      bool tiling_t::operator==(const tiling_t& other) const
      {
         return tiles == other.tiles && t1 == other.t1 && t2 == other.t2;
      }

      rectangle_t tiling_t::bounds() const
      {
         rectangle_t tiling_bounds;
         for (const auto& poly_trf : tiles)
         {
            for (const auto& trf : poly_trf.second)
            {
               const auto poly_bounds = poly_trf.first.apply(trf).bounds();
               if (tiling_bounds.is_invalid())
                  tiling_bounds = poly_bounds;
               else
                  tiling_bounds = tiling_bounds.combine(poly_bounds);
            }
         }
         return tiling_bounds;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
