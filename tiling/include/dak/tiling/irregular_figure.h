#pragma once

#ifndef DAK_TILING_IRREBULAR_FIGURE_H
#define DAK_TILING_IRREGULAR_FIGURE_H

#include <dak/tiling/figure.h>
#include <dak/tiling/infer_mode.h>
#include <dak/tiling/mosaic.h>

#include <dak/geometry/polygon.h>

#include <algorithm>

namespace dak
{
   namespace tiling
   {
      using geometry::polygon;
      class infer_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Figure based on a non-regular polygon.

      class irregular_figure_t : public figure_t
      {
      public:
         std::shared_ptr<dak::tiling::mosaic_t> mosaic;
         polygon poly;

         infer_mode_t infer = infer_mode_t::girih;
         double q = 0.;
         double d = 1.;
         int    s = 1;

         irregular_figure_t() { }
         irregular_figure_t(const std::shared_ptr<dak::tiling::mosaic_t>& mo, const polygon& p) : irregular_figure_t(mo, p, infer_mode_t::girih) { }
         irregular_figure_t(const std::shared_ptr<dak::tiling::mosaic_t>& mo, const polygon& p, infer_mode_t i) : irregular_figure_t(mo, p, i, std::max(0.333, p.points.size() / 1.7)) { }
         irregular_figure_t(const std::shared_ptr<dak::tiling::mosaic_t>& mo, const polygon& p, infer_mode_t i, double d) : mosaic(mo), poly(p), infer(i), d(d) { }

         // Copy a figure.
         std::shared_ptr<figure_t> clone() const override;
         void make_similar(const figure_t& other) override;

         // Figure implementation.
         bool is_similar(const figure_t& other) const override;

         // Comparison.
         bool operator==(const figure_t& other) const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // Figure cache implementation.
         bool is_cache_valid() const override;
         void update_cached_values() const override;
         void build_map() const;

      private:
         mutable polygon cached_poly;
         mutable infer_mode_t cached_infer = infer_mode_t::girih;

         mutable double cached_q = NAN;
         mutable double cached_d = NAN;
         mutable int    cached_s = -1;

         friend class infer_t;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
