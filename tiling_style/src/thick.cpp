#include <dak/tiling_style/thick.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

namespace dak
{
   namespace tiling_style
   {
      using ui::stroke_t;
      using utility::L;

      std::shared_ptr<layer_t> thick_t::clone() const
      {
         return std::make_shared<thick_t>(*this);
      }

      void thick_t::make_similar(const layer_t& other)
      {
         colored_t::make_similar(other);

         if (const thick_t* other_thick = dynamic_cast<const thick_t*>(&other))
         {
            width         = other_thick->width;
            outline_width = other_thick->outline_width;
            outline_color = other_thick->outline_color;
            join          = other_thick->join;
         }
      }

      std::wstring thick_t::describe() const
      {
         return L::t(L"Thick");
      }

      ui::stroke_t thick_t::get_stroke(ui::drawing_t& drw, double sw) const
      {
         const double w = drw.get_transform().dist_from_zero(sw);
         return stroke_t(w,
            join == stroke_t::join_style_t::round ? stroke_t::cap_style_t::round : stroke_t::cap_style_t::flat,
            join);
      }

      void thick_t::draw_edges(ui::drawing_t& drw, double width) const
      {
         const geometry::edges_map_t::edges_t& edges = map.all();
         for (const auto &e : edges)
         {
            drw.set_stroke(get_stroke(drw, get_width_at(e.p2, width)));
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

      void thick_t::internal_draw(ui::drawing_t& drw)
      {
         // Note: we multiply the width by two because all other styles using
         //       the width actully widen the drawing in both perpendicular
         //       directions by that width.
         if (!utility::near_zero(outline_width))
         {
            drw.set_color(outline_color);
            draw_edges(drw, outline_width + width * 2.);
         }
         drw.set_color(color);
         draw_edges(drw, width * 2);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
