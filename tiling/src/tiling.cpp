#include <dak/tiling/tiling.h>

namespace dak
{
   namespace tiling
   {
      tiling_t::tiling_t()
      {
      }

      tiling_t::tiling_t(const std::wstring& name)
         : name(name)
      {
      }

      void tiling_t::swap(tiling_t& other)
      {
         tiles.swap(other.tiles);
         name.swap(other.name);
         description.swap(other.description);
         author.swap(other.author);
      }

      bool tiling_t::operator==(const tiling_t& other) const
      {
         return tiles == other.tiles;
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

      bool tiling_t::is_invalid() const
      {
         return tiles.empty();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
