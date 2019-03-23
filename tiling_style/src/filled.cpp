#include <dak/tiling_style/filled.h>

namespace dak
{
   namespace tiling_style
   {
      std::shared_ptr<layer> filled::clone() const
      {
         return std::make_shared<filled>(*this);
      }

      void filled::make_similar(const layer& other)
      {
         colored::make_similar(other);

         if (const filled* other_filled = dynamic_cast<const filled*>(&other))
         {
            draw_inside  = other_filled->draw_inside;
            draw_outside = other_filled->draw_outside;
         }
      }

      std::wstring filled::describe() const
      {
         return geometry::L::t(L"Filled");
      }

      void filled::set_map(const geometry::map& m)
      {
         cached_inside.clear();
         cached_outside.clear();
         cached_odd.clear();
         colored::set_map(m);
      }

      // The internal draw is called with the layer transform already applied.
      void filled::internal_draw(ui::drawing& drw)
      {
         if (cached_inside.empty())
         {
            geometry::face::faces exteriors;
            geometry::face::make_faces(map, cached_inside, cached_outside, cached_odd, exteriors);
         }

         drw.set_color(color);
         if (draw_inside)
            for (const auto& f : cached_inside)
               drw.fill_polygon(f);
         if (draw_outside)
            for (const auto& f : cached_outside)
               drw.fill_polygon(f);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
