#include <dak/tiling_style/emboss.h>

#include <dak/geometry/utility.h>

#include <dak/ui/drawing.h>

#include <cmath>

namespace dak
{
   namespace tiling_style
   {
      std::shared_ptr<layer> emboss::clone() const
      {
         return std::make_shared<emboss>(*this);
      }

      void emboss::make_similar(const layer& other)
      {
         outline::make_similar(other);

         if (const emboss* other_emboss = dynamic_cast<const emboss*>(&other))
         {
            angle = other_emboss->angle;
         }
      }

      std::wstring emboss::describe() const
      {
         return geometry::L::t(L"Embossed");
      }

      void emboss::internal_draw_fat_lines(ui::drawing& drw, const fat_lines& fat_lines)
      {
         ui::color greys[17] =
         {
            ui::color::black(), ui::color::black(), ui::color::black(), ui::color::black(),
            ui::color::black(), ui::color::black(), ui::color::black(), ui::color::black(),
            ui::color::black(), ui::color::black(), ui::color::black(), ui::color::black(),
            ui::color::black(), ui::color::black(), ui::color::black(), ui::color::black(),
            ui::color::black(),
         };

         for (int idx = 0; idx < 17; ++idx)
         {
            double t = idx / 16.;
            greys[idx] = ui::color(ui::color::channel(color.r * t), ui::color::channel(color.g * t), ui::color::channel(color.b * t), 255);
         }

         const point light(std::cos(angle), std::sin(angle));
         for (const auto& fat_line : fat_lines)
         {
            const std::vector<point>& pts = fat_line.hexagon.points;
            draw_trap(drw, pts[1], pts[2], pts[3], pts[4], light, greys);
            draw_trap(drw, pts[4], pts[5], pts[0], pts[1], light, greys);
         }

         if (geometry::near_zero(outline_width))
            return;

         drw.set_stroke(ui::stroke(1.));
         drw.set_color(outline_color);
         for (const auto& fat_line : fat_lines)
         {
            drw.draw_polygon(fat_line.hexagon);
            const std::vector<point>& pts = fat_line.hexagon.points;
            drw.draw_line(pts[1], pts[4]);
         }
      }

      void emboss::draw_trap(ui::drawing& drw, const point& a, const point& b, const point& c, const point& d, const point& light, const ui::color* greys)
      {
         const point N = (a - d).perp().normalize();

         // dd is a normalized floating point value corresponding to 
         // the brightness to use.
         const double dd = 0.5 * (N.x * light.x + N.y * light.y + 1.0);

         // Quantize to sixteen grey values.
         const int bb = (int) std::round(16. * dd);
         drw.set_color(greys[bb]);

         const geometry::polygon poly({ a, b, c, d });
         drw.fill_polygon(poly);
      }

      std::pair<point, point> emboss::get_points_many_connections(const edge& an_edge, size_t index, double width, const geometry::map::range& connections)
      {
         return get_points_intersection(an_edge, index, width, width, connections);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
