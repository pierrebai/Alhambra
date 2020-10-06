#include <dak/tiling_style/plain.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

namespace dak
{
   namespace tiling_style
   {
      using ui::stroke_t;
      using utility::L;

      std::shared_ptr<layer_t> plain_t::clone() const
      {
         return std::make_shared<plain_t>(*this);
      }

      std::wstring plain_t::describe() const
      {
         return L::t(L"Plain");
      }

      // The internal draw is called with the layer transform already applied.
      void plain_t::internal_draw(ui::drawing_t& drw)
      {
         drw.set_color(color);
         drw.set_stroke(stroke_t(1., stroke_t::cap_style_t::round, stroke_t::join_style_t::round));
         for (const auto& e : map.all())
            if (e.is_canonical())
               drw.draw_line(e.p1, e.p2);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
