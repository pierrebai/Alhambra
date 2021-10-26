#include <dak/tiling_style/filled.h>

#include <dak/ui/drawing.h>

#include <dak/utility/text.h>

namespace dak
{
   namespace tiling_style
   {
      using utility::L;

      std::shared_ptr<layer_t> filled_t::clone() const
      {
         return std::make_shared<filled_t>(*this);
      }

      void filled_t::make_similar(const layer_t& other)
      {
         colored_t::make_similar(other);

         if (const filled_t* other_filled = dynamic_cast<const filled_t*>(&other))
         {
            draw_inside  = other_filled->draw_inside;
            draw_outside = other_filled->draw_outside;
         }
      }

      bool filled_t::operator==(const layer_t& other) const
      {
         if (!colored_t::operator==(other))
            return false;


         if (const filled_t* other_filled = dynamic_cast<const filled_t*>(&other))
         {
            return draw_inside == other_filled->draw_inside &&
                   draw_outside == other_filled->draw_outside;
         }

         return false;
      }

      std::wstring filled_t::describe() const
      {
         return L::t(L"Filled");
      }

      void filled_t::set_map(const geometry::edges_map_t& m, const std::shared_ptr<const tiling_t>& t)
      {
         cached_inside.clear();
         cached_outside.clear();
         cached_odd.clear();
         colored_t::set_map(m, t);
      }

      // The internal draw is called with the layer transform already applied.
      void filled_t::internal_draw(ui::drawing_t& drw)
      {
         if (cached_inside.empty())
         {
            cached_outside.clear();
            cached_odd.clear();
            geometry::face_t::faces_t exteriors;
            geometry::face_t::make_faces(map, cached_inside, cached_outside, cached_odd, exteriors);
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
