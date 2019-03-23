#include <dak/ui/color.h>

namespace dak
{
   namespace ui
   {
      color::channel random_colors::next_color()
      {
         return color::channel(80. + 175. * rand() / (double) rand.max());
      };

      color random_colors::any()
      {
         return color(next_color(), next_color(), next_color(), 255);
      }

      color random_colors::red()
      {
         return color(next_color(), 0, 0, 255);
      }

      color random_colors::green()
      {
         return color(0, next_color(), 0, 255);
      }

      color random_colors::blue()
      {
         return color(0, 0, next_color(), 255);
      }

      color random_colors::cyan()
      {
         return color(0, next_color(), next_color(), 255);
      }

      color random_colors::yellow()
      {
         return color(next_color(), next_color(), 0, 255);
      }

      color random_colors::magenta()
      {
         return color(next_color(), 0, next_color(), 255);
      }

      color random_colors::grey()
      {
         const color::channel c = next_color();
         return color(c, c, c, 255);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
