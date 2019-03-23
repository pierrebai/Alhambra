#pragma once

#ifndef DAK_TILING_SCALE_FIGURE_H
#define DAK_TILING_SCALE_FIGURE_H

#include <dak/tiling/radial_figure.h>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Scale a child figure

      class scale_figure : public radial_figure
      {
      public:
         scale_figure(int n) : radial_figure(n) { }

         // Radial figure implementation.
         map build_unit() const override;

      protected:
         // Return the child radial figure that is being scaled.
         virtual const radial_figure* get_child() const = 0;

         // Compute the scale to be applied to the child's build unit.
         virtual double compute_scale() const = 0;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
