#include <dak/ui/drawing.h>

#include <dak/tiling/mosaic.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      void draw_tiling(dak::ui::drawing_t& drw, const tiling::tiling_t& tiling, const ui::color_t& co, int copy_count);
      void draw_tiling(dak::ui::drawing_t& drw, const std::shared_ptr<tiling::mosaic_t>& mosaic, const ui::color_t& co, int copy_count);
   }
}

// vim: sw=3 : sts=3 : et : sta : 
