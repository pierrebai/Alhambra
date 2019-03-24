#pragma once

#ifndef DAK_TILING_IRREBULAR_FIGURE_H
#define DAK_TILING_IRREGULAR_FIGURE_H

#include <dak/tiling/figure.h>
#include <dak/tiling/infer_mode.h>
#include <dak/tiling/mosaic.h>

#include <dak/geometry/polygon.h>

namespace dak
{
   namespace tiling
   {
      using geometry::polygon;
      class infer;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Figure based on a non-regular polygon.

      class irregular_figure : public figure
      {
      public:
         std::shared_ptr<dak::tiling::mosaic> mosaic;
         polygon poly;

         infer_mode infer = infer_mode::girih;
         double q = 0.;
         double d = 0.6;
         int    s = 1;

         irregular_figure() { }
         irregular_figure(const std::shared_ptr<dak::tiling::mosaic>& mo, const polygon& p) : mosaic(mo), poly(p) { }
         irregular_figure(const std::shared_ptr<dak::tiling::mosaic>& mo, const polygon& p, infer_mode i) : mosaic(mo), poly(p), infer(i) { }

         // Copy a figure.
         std::shared_ptr<figure> clone() const override;
         void make_similar(const figure& other) override;

         // Figure implementation.
         bool is_similar(const figure& other) const override;

         // Comparison.
         bool operator==(const figure& other) const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // Figure cache implementation.
         bool is_cache_valid() const override;
         void update_cached_values() const override;
         void build_map() const;

      private:
         mutable polygon cached_poly;
         mutable infer_mode cached_infer = infer_mode::girih;

         mutable double cached_q = NAN;
         mutable double cached_d = NAN;
         mutable int    cached_s = -1;

         friend class infer;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
