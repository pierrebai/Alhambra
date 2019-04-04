#include <dak/tiling_style/sketch.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

#include <random>

namespace dak
{
   namespace tiling_style
   {
      using geometry::point;
      using utility::L;

      std::shared_ptr<layer> sketch::clone() const
      {
         return std::make_shared<sketch>(*this);
      }

      std::wstring sketch::describe() const
      {
         return L::t(L"Sketched");
      }

      void sketch::internal_draw(ui::drawing& drw)
      {
         drw.set_color(color);
         drw.set_stroke(ui::stroke(1.));

         std::minstd_rand rand;

         const double val = drw.get_transform().dist_from_inverted_zero(15.0);
         const point jitter(val, val);
         const point halfjit(val / 2, val / 2);
         for (const auto& e : map.all())
         {
            if (!e.is_canonical())
               continue;

            const point p1 = e.p1 - halfjit;
            const point p2 = e.p2 - halfjit;

            for (int c = 0; c < 8; ++c)
            {
               drw.draw_line(p1 + jitter.scale(rand() / (double) rand.max()), p2 + jitter.scale(rand() / (double) rand.max()));
            }
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
