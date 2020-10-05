#include <dak/tiling_style/styled_mosaic.h>
#include <dak/tiling_style/plain.h>

namespace dak
{
   namespace tiling_style
   {
      styled_mosaic_t::styled_mosaic_t(const styled_mosaic_t& other)
      : layer(other)
      , mosaic(std::make_shared<tiling::mosaic>(other.mosaic ? *other.mosaic : tiling::mosaic()))
      , style_t(other.style ? std::dynamic_pointer_cast<tiling_style::style>(other.style->clone()) : std::shared_ptr<tiling_style::style>(new plain))
      {
      }

      void styled_mosaic_t::make_similar(const layer& other)
      {
         if (style_t)
         {
            if (const styled_mosaic_t* other_mosaic_layer = dynamic_cast<const styled_mosaic_t*>(&other))
            {
               if (other_mosaic_layer->style)
                  style_t->make_similar(*other_mosaic_layer->style);
            }
            else if (const tiling_style::style_t* other_style = dynamic_cast<const tiling_style::style_t*>(&other))
            {
               style_t->make_similar(*other_style);
            }
         }
      }

      styled_mosaic_t& styled_mosaic_t::operator=(const styled_mosaic_t& other)
      {
         layer::operator=(other);
         mosaic = std::make_shared<tiling::mosaic_t>(other.mosaic ? *other.mosaic : tiling::mosaic_t());
         style_t = other.style ? std::dynamic_pointer_cast<tiling_style::style_t>(other.style->clone()) : std::shared_ptr<tiling_style::style_t>(new plain_t);
         return *this;
      }

      std::shared_ptr<layer> styled_mosaic::clone() const
      {
         return std::make_shared<styled_mosaic>(*this);
      }

      void styled_mosaic::update_style(const rect& region)
      {
         if (!style)
            return;

         if (!mosaic)
            return;

         style->set_map(mosaic->construct(region));
      }

      void styled_mosaic::internal_draw(ui::drawing& drw)
      {
         if (!style)
            return;

         style->draw(drw);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
