#include <dak/tiling_style/thick.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

namespace dak
{
   namespace tiling_style
   {
      using ui::stroke;
      using utility::L;

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
         return L::t(L"Thick");
      }

      ui::stroke thick::get_stroke(ui::drawing& drw, double sw) const
      {
         const double w = drw.get_transform().dist_from_zero(sw);
         return stroke(w,
            join == stroke::join_style::round ? stroke::cap_style::round : stroke::cap_style::flat,
            join);
      }

      static void draw_edges(ui::drawing& drw, const geometry::map& map, const geometry::map::edges& edges)
      {
         for (const auto &e : edges)
         {
            const auto e2 = map.continuation(e);
            if (e2.is_invalid())
            {
               drw.draw_line(e.p1, e.p2);
            }
            else
            {
               const auto p1 = e.p1.convex_sum(e.p2, 0.4);
               const auto p3 = e2.p1.convex_sum(e2.p2, 0.6);
               drw.draw_corner(p1, e.p2, p3);
            }
         }
      }

      void thick::internal_draw(ui::drawing& drw)
      {
         // Note: we multiply the width by two because all other styles using
         //       the width actully widen the drawing in both perpendicular
         //       directions by that width.
         if (!utility::near_zero(outline_width))
         {
            drw.set_color(outline_color);
            drw.set_stroke(get_stroke(drw, outline_width + width * 2.));
            draw_edges(drw, map, map.all());
         }
         drw.set_color(color);
         drw.set_stroke(get_stroke(drw, width * 2));
         draw_edges(drw, map, map.all());
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
