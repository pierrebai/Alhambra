#include <dak/tiling_style/sketch.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

#include <random>

namespace dak
{
   namespace tiling_style
   {
      using geometry::point_t;
      using utility::L;

      std::shared_ptr<layer_t> sketch_t::clone() const
      {
         return std::make_shared<sketch_t>(*this);
      }

      std::wstring sketch_t::describe() const
      {
         return L::t(L"Sketched");
      }

      void sketch_t::internal_draw(ui::drawing_t& drw)
      {
         drw.set_color(color);
         drw.set_stroke(ui::stroke_t(1.));

         std::minstd_rand rand;

         const double val = drw.get_transform().dist_from_inverted_zero(15.0);
         const point_t jitter(val, val);
         const point_t halfjit(val / 2, val / 2);
         for (const auto& e : map.all())
         {
            if (!e.is_canonical())
               continue;

            const point_t p1 = e.p1 - halfjit;
            const point_t p2 = e.p2 - halfjit;

            for (int c = 0; c < 8; ++c)
            {
               drw.draw_line(p1 + jitter.scale(rand() / (double) rand.max()), p2 + jitter.scale(rand() / (double) rand.max()));
            }
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
