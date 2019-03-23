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

      class star : public radial_figure
      {
      public:
         double d = 3.;
         int    s = 2;

         star() : radial_figure(8) { }
         star(int n) : radial_figure(n) { }
         star(int n, double d, int s) : radial_figure(n), d(d), s(s) { }

         // Copy a figure.
         std::shared_ptr<figure> clone() const override;
         void make_similar(const figure& other) override;

         // Figure implementation.
         bool is_similar(const figure& other) const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

         // Radial figure implementation.
         map build_unit() const override;

      protected:
         // Figure cache implementation.
         bool is_cache_valid() const override;
         void update_cached_values() const override;

      private:
         mutable double cached_d_last_build_unit = NAN;
         mutable int cached_s_last_build_unit = -1;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
