#include <dak/ui/drawing_base.h>

namespace dak
{
   namespace ui
   {
      stroke drawing_base::get_stroke() const
      {
         return strk;
      }

      drawing_base& drawing_base::set_stroke(const stroke& s)
      {
         strk = s;
         return *this;
      }

      color drawing_base::get_color() const
      {
         return co;
      }

      drawing_base& drawing_base::set_color(const color& c)
      {
         co = c;
         return *this;
      }

      const transform& drawing_base::get_transform() const
      {
         return trf;
      }

      drawing_base& drawing_base::set_transform(const transform& t)
      {
         trf = t;
         return *this;
      }

      drawing_base& drawing_base::compose(const transform& t)
      {
         trf = trf.compose(t);
         return *this;
      }

      drawing_base& drawing_base::push_transform()
      {
         saved_trfs.emplace_back(trf);
         return *this;
      }

      drawing_base& drawing_base::pop_transform()
      {
         if (saved_trfs.size() <= 0)
            return *this;

         trf = saved_trfs.back();
         saved_trfs.pop_back();
         return *this;
      }

      static polygon make_arrow(const point& from, const point& to, double length, double half_width)
      {
         const point dir = (to - from).normalize().scale(length);
         const point perp = dir.perp().normalize().scale(half_width);
         return polygon({ to, to - dir + perp, to - dir - perp, to });
      }

      void drawing::draw_arrow(const point& from, const point& to, double length, double half_width)
      {
         draw_polygon(make_arrow(from, to, length, half_width));
      }

      void drawing::fill_arrow(const point& from, const point& to, double length, double half_width)
      {
         fill_polygon(make_arrow(from, to, length, half_width));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
