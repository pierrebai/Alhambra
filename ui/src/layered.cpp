#include <dak/ui/layered.h>
#include <dak/ui/drawing.h>

namespace dak
{
   namespace ui
   {
      void layered::set_layers(const layers& l)
      {
         _layers = l;
      }

      void layered::draw(drawing& drw)
      {
         const auto end = _layers.rend();
         for (auto i = _layers.rbegin(); i != end; ++i)
         {
            drw.push_transform();
            drw.compose(trf);
            (*i)->draw(drw);
            drw.pop_transform();
         }
      }

      const transform& layered::get_transform() const
      {
         return trf;
      }

      layered& layered::set_transform(const transform& t)
      {
         trf = t;
         return *this;
      }

      layered& layered::compose(const transform& t)
      {
         trf = trf.compose(t);
         return *this;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
