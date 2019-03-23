#include <dak/ui_qt/painter_transformable.h>

namespace dak
{
   namespace ui_qt
   {
      const transform& painter_transformable::get_transform() const
      {
         return painter_drawing::get_transform();
      }

      painter_transformable& painter_transformable::set_transform(const transform& t)
      {
         painter_drawing::set_transform(t);
         return *this;
      }

      painter_transformable& painter_transformable::compose(const transform& t)
      {
         painter_drawing::compose(t);
         return *this;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
