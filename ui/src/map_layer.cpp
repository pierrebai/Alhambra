#include <dak/ui/map_layer.h>

#include <dak/ui/drawing.h>

namespace dak
{
   namespace ui
   {
      std::shared_ptr<layer> map_layer::clone() const
      {
         return std::make_shared<map_layer>(*this);
      }

      void map_layer::make_similar(const layer& other)
      {
         layer::make_similar(other);

         if (const map_layer* other_map_layer = dynamic_cast<const map_layer*>(&other))
         {
            map = other_map_layer->map;
         }
      }

      void map_layer::internal_draw(drawing& drw)
      {
         drw.set_color(color::black());
         drw.set_stroke(stroke(1.));
         for (const auto edge : map.canonicals())
            drw.draw_line(edge.p1, edge.p2);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
