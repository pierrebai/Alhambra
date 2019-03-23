#pragma once

#ifndef DAK_UI_TRANSFORMABLE_H
#define DAK_UI_TRANSFORMABLE_H

#include <dak/geometry/point.h>
#include <dak/geometry/transform.h>

namespace dak
{
   namespace ui
   {
      using geometry::point;
      using geometry::transform;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Interface needed to enable transform something.
      //
      // Used by the transformer class.

      class transformable
      {
      public:
         virtual ~transformable() = 0 { }

         // Changing the transform.
         virtual const transform& get_transform() const = 0;
         virtual transformable& set_transform(const transform&) = 0;
         virtual transformable& compose(const transform&) = 0;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
