#include <dak/tiling_style/thick.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_style
   {
      using ui::stroke;

      std::shared_ptr<layer> thick::clone() const
      {
         return std::make_shared<thick>(*this);
      }

      void thick::make_similar(const layer& other)
      {
         colored::make_similar(other);

         if (const thick* other_thick = dynamic_cast<const thick*>(&other))
         {
            width         = other_thick->width;
            outline_width = other_thick->outline_width;
            outline_color = other_thick->outline_color;
         }
      }

      std::wstring thick::describe() const
      {
         return geometry::L::t(L"Thick");
      }

      void thick::internal_draw(ui::drawing& drw)
      {
         // Note: we multiply the width by two because all other styles using
         //       the width actully widen the drawing in both perpendicular
         //       directions by that width.
         if (!geometry::near_zero(outline_width))
         {
            const double ow = drw.get_transform().dist_from_zero(outline_width);
            drw.set_color(outline_color);
            drw.set_stroke(stroke(outline_width));
            for (const auto &e : map.canonicals())
               drw.draw_line(e.p1, e.p2);
         }
         drw.set_color(color);
         const double w = drw.get_transform().dist_from_zero(width);
         drw.set_stroke(ui::stroke(w));
         for (const auto &e : map.canonicals())
            drw.draw_line(e.p1, e.p2);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
