#include <dak/tiling_style/styled_mosaic.h>
#include <dak/tiling_style/plain.h>

namespace dak
{
   namespace tiling_style
   {
      styled_mosaic_t::styled_mosaic_t(const styled_mosaic_t& other)
      : layer_t(other)
      , mosaic(std::make_shared<tiling::mosaic_t>(other.mosaic ? *other.mosaic : tiling::mosaic_t()))
      , style(other.style ? std::dynamic_pointer_cast<tiling_style::style_t>(other.style->clone()) : std::shared_ptr<tiling_style::style_t>(new plain_t))
      {
      }

      void styled_mosaic_t::make_similar(const layer_t& other)
      {
         if (style)
         {
            if (const styled_mosaic_t* other_mosaic_layer = dynamic_cast<const styled_mosaic_t*>(&other))
            {
               if (other_mosaic_layer->style)
                  style->make_similar(*other_mosaic_layer->style);
            }
            else if (const tiling_style::style_t* other_style = dynamic_cast<const tiling_style::style_t*>(&other))
            {
               style->make_similar(*other_style);
            }
         }
      }

      styled_mosaic_t& styled_mosaic_t::operator=(const styled_mosaic_t& other)
      {
         layer_t::operator=(other);
         mosaic = std::make_shared<tiling::mosaic_t>(other.mosaic ? *other.mosaic : tiling::mosaic_t());
         style = other.style ? std::dynamic_pointer_cast<tiling_style::style_t>(other.style->clone()) : std::shared_ptr<tiling_style::style_t>(new plain_t);
         return *this;
      }

      std::shared_ptr<layer_t> styled_mosaic_t::clone() const
      {
         return std::make_shared<styled_mosaic_t>(*this);
      }

      void styled_mosaic_t::update_style(const rectangle_t& region)
      {
         if (!style)
            return;

         if (!mosaic)
            return;

         style->set_map(mosaic->construct(region), mosaic->tiling);
      }

      void styled_mosaic_t::internal_draw(ui::drawing_t& drw)
      {
         if (!style)
            return;

         style->draw(drw);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
