#include <dak/ui/layer.h>
#include <dak/ui/drawing.h>

namespace dak
{
   namespace ui
   {
      void layer::make_similar(const layer& other)
      {
         trf = other.trf;
      }

      void layer::draw(drawing& drw)
      {
         if (hide)
            return;

         drw.push_transform();
         drw.compose(trf);
         internal_draw(drw);
         drw.pop_transform();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
