#include <dak/ui_qt/paint_device_transformable.h>

namespace dak
{
   namespace ui_qt
   {
      const transform& paint_device_transformable::get_transform() const
      {
         return paint_device_drawing::get_transform();
      }

      paint_device_transformable& paint_device_transformable::set_transform(const transform& t)
      {
         paint_device_drawing::set_transform(t);
         return *this;
      }

      paint_device_transformable& paint_device_transformable::compose(const transform& t)
      {
         paint_device_drawing::compose(t);
         return *this;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
