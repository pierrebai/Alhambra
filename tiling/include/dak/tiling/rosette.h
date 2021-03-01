#pragma once

#ifndef DAK_TILING_ROSETTE_H
#define DAK_TILING_ROSETTE_H

#include <dak/tiling/radial_figure.h>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // The rosette is a classic feature of Islamic art.  It's a star
      // shape surrounded by hexagons.
      //
      // This class implements the rosette as a radial_figure using the 
      // geometric construction given in Lee [1].
      //
      // [1] A.J. Lee, _Islamic Star Patterns_.  Muqarnas 4.

      class rosette_t : public radial_figure_t
      {
      public:
         double  q = 0.;
         int     s = 2;

         rosette_t() : radial_figure_t(6) { }
         rosette_t(int n) : radial_figure_t(n) { }
         rosette_t(int n, double q, int s);

         // Copy a figure.
         std::shared_ptr<figure_t> clone() const override;
         void make_similar(const figure_t& other) override;

         // Figure implementation.
         bool is_similar(const figure_t& other) const override;

         // Comparison.
         bool operator==(const figure_t& other) const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

         // Radial figure implementation.
         edges_map_t build_unit() const override;

      protected:
         // Figure cache implementation.
         bool is_cache_valid() const override;
         void update_cached_values() const override;

      private:
         mutable double cached_q_last_build_unit = NAN;
         mutable int cached_s_last_build_unit = -1;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

