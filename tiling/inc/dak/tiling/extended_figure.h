#pragma once

#ifndef DAK_TILING_EXTENDED_FIGURE_H
#define DAK_TILING_EXTENDED_FIGURE_H

#include <dak/tiling/scale_figure.h>

#include <memory>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // An extended figure is a special kind of scale figure.  It knows how to
      // compute just the right scale factor so that scaled out edges will join
      // up to create a fancier figure.  This is how we turn rosettes into
      // extended rosettes.  To make sure that the resulting figure still lines
      // up with the feature that will eventually contain it, we need to do
      // some fancy reshuffling of the basic unit to move the apex to (1,0).

      class extended_figure : public scale_figure
      {
      public:
         std::shared_ptr<radial_figure> child;

         extended_figure(const std::shared_ptr<radial_figure>& child);
         extended_figure(const extended_figure& other);
         extended_figure& operator=(const extended_figure& other);

         // Copy a figure.
         std::shared_ptr<figure> clone() const override;
         void make_similar(const figure& other) override;

         void child_changed();

         // Figure implementation.
         bool is_similar(const figure& other) const override;

         // Comparison.
         bool operator==(const figure& other) const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

         // Radial figure implementation.
         map build_unit() const override;

      protected:
         // Figure cache implementation.
         bool is_cache_valid() const override;
         void update_cached_values() const override;

         // Scale figure implementation.
         const radial_figure* get_child() const override;
         double compute_scale() const override;

      private:
         static double compute_scale(std::shared_ptr<radial_figure> child);
         static void scale_to_unit(map& cunit);

         mutable double cached_s_last_build_unit = NAN;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

