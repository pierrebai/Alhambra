#include <dak/tiling_style/plain.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_style
   {
      using ui::stroke;

      std::shared_ptr<layer> plain::clone() const
      {
         return std::make_shared<plain>(*this);
      }

      std::wstring plain::describe() const
      {
         return geometry::L::t(L"Plain");
      }

      // The internal draw is called with the layer transform already applied.
      void plain::internal_draw(ui::drawing& drw)
      {
         drw.set_color(color);
         drw.set_stroke(stroke(1., stroke::cap_style::round, stroke::join_style::round));
         for (const auto& e : map.canonicals())
            drw.draw_line(e.p1, e.p2);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
