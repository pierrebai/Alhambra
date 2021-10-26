#include <dak/tiling_style/emboss.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

#include <cmath>

namespace dak
{
   namespace tiling_style
   {
      using utility::L;

      std::shared_ptr<layer_t> emboss_t::clone() const
      {
         return std::make_shared<emboss_t>(*this);
      }

      void emboss_t::make_similar(const layer_t& other)
      {
         outline_t::make_similar(other);

         if (const emboss_t* other_emboss = dynamic_cast<const emboss_t*>(&other))
         {
            angle = other_emboss->angle;
         }
      }

      bool emboss_t::operator==(const layer_t& other) const
      {
         if (!outline_t::operator==(other))
            return false;

         if (const emboss_t* other_emboss = dynamic_cast<const emboss_t*>(&other))
         {
            return dak::utility::near(angle, other_emboss->angle);
         }

         return false;
      }

      std::wstring emboss_t::describe() const
      {
         return L::t(L"Embossed");
      }

      void emboss_t::internal_draw_fat_lines(ui::drawing_t& drw, const fat_lines_t& fat_lines)
      {
         ui::color_t greys[17] =
         {
            ui::color_t::black(), ui::color_t::black(), ui::color_t::black(), ui::color_t::black(),
            ui::color_t::black(), ui::color_t::black(), ui::color_t::black(), ui::color_t::black(),
            ui::color_t::black(), ui::color_t::black(), ui::color_t::black(), ui::color_t::black(),
            ui::color_t::black(), ui::color_t::black(), ui::color_t::black(), ui::color_t::black(),
            ui::color_t::black(),
         };

         for (int idx = 0; idx < 17; ++idx)
         {
            double t = idx / 16.;
            greys[idx] = ui::color_t(ui::color_t::channel(color.r * t), ui::color_t::channel(color.g * t), ui::color_t::channel(color.b * t), 255);
         }

         const point_t light(std::cos(angle), std::sin(angle));
         for (const auto& fat_line : fat_lines)
         {
            const std::vector<point_t>& pts = fat_line.hexagon.points;
            draw_trap(drw, pts[1], pts[2], pts[3], pts[4], light, greys);
            draw_trap(drw, pts[4], pts[5], pts[0], pts[1], light, greys);
         }

         if (utility::near_zero(outline_width))
            return;

         drw.set_stroke(ui::stroke_t(1.));
         drw.set_color(outline_color);
         for (const auto& fat_line : fat_lines)
         {
            drw.draw_polygon(fat_line.hexagon);
            const std::vector<point_t>& pts = fat_line.hexagon.points;
            drw.draw_line(pts[1], pts[4]);
         }
      }

      void emboss_t::draw_trap(ui::drawing_t& drw, const point_t& a, const point_t& b, const point_t& c, const point_t& d, const point_t& light, const ui::color_t* greys)
      {
         const point_t N = (a - d).perp().normalize();

         // dd is a normalized floating point value corresponding to 
         // the brightness to use.
         const double dd = 0.5 * (N.x * light.x + N.y * light.y + 1.0);

         // Quantize to sixteen grey values.
         const int bb = (int) std::round(16. * dd);
         drw.set_color(greys[bb]);

         const geometry::polygon_t poly({ a, b, c, d });
         drw.fill_polygon(poly);
      }

      std::pair<point_t, point_t> emboss_t::get_points_many_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections)
      {
         return get_points_intersection(an_edge, index, width, width, connections);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
