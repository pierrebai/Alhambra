#pragma once

#ifndef DAK_TILING_STAR_H
#define DAK_TILING_STAR_H

#include <dak/tiling/radial_figure.h>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      // 
      // The classic [n/d]s star construction.  See the paper for more
      // details.

      class star_t : public radial_figure_t
      {
      public:
         double d = 3.;
         int    s = 2;

         star_t() : radial_figure_t(8) { }
         star_t(int n) : radial_figure_t(n) { }
         star_t(int n, double d, int s) : radial_figure_t(n), d(d), s(s) { }

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
         mutable double my_cached_d_last_build_unit = NAN;
         mutable int my_cached_s_last_build_unit = -1;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
