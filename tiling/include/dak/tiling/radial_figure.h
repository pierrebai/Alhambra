#pragma once

#ifndef DAK_TILING_RADIAL_FIGURE_H
#define DAK_TILING_RADIAL_FIGURE_H

#include <dak/tiling/figure.h>

namespace dak
{
   namespace tiling
   {
      class extended_figure_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A radial figure is a special kind of figure that has n symmetry.  That
      // means that it can be rotated by 360/n degrees and flipped across certain
      // lines through the origin and it looks the same.
      //
      // We take advantage of this by only making subclasses produce a basic
      // unit, i.e. a smaller map that generates the complete figure through the
      // action of the rotations.

      class radial_figure_t : public figure_t
      {
      public:
         int n = 6;

         radial_figure_t() : radial_figure_t(6) { }
         radial_figure_t(int n) : n(n) { }

         // Subclasses provide a method for getting the basic unit.
         // The unit *must* have a point at point::unit_x (1, 0).
         virtual edges_map_t build_unit() const = 0;

      protected:
         // Figure cache implementation.
         bool is_cache_valid() const override;
         void update_cached_values() const override;
         void build_map() const override;

      private:
         mutable int my_cached_n_last_build_unit = -1;

         friend class extended_figure_t;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
