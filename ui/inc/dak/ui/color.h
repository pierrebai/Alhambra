#pragma once

#ifndef DAK_UI_COLOR_H
#define DAK_UI_COLOR_H

#include <random>

namespace dak
{
   namespace ui
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // For channel color: red, green, blue and alpha. 8-bit values.

      class color
      {
      public:
         typedef unsigned char channel;
         channel r = 0;
         channel g = 0;
         channel b = 0;
         channel a = 0;

         constexpr color(channel r, channel g, channel b, channel a) : r(r), g(g), b(b), a(a) { }

         static constexpr color black()    { return color(  0,   0,   0, 255); }
         static constexpr color white()    { return color(255, 255, 255, 255); }
         static constexpr color red()      { return color(255,   0,   0, 255); }
         static constexpr color green()    { return color(  0, 255,   0, 255); }
         static constexpr color blue()     { return color(  0,   0, 255, 255); }
         static constexpr color cyan()     { return color(  0, 255, 255, 255); }
         static constexpr color yellow()   { return color(255, 255,   0, 255); }
         static constexpr color magenta()  { return color(255,   0, 255, 255); }
         static constexpr color grey()     { return color(160, 160, 160, 255); }

         color percent(double pc) const { return color(channel(r * pc / 100), channel(g * pc / 100), channel(b * pc / 100), a); }
         color operator+(const color& other) const { return color(r + other.r, g + other.g, b + other.b, a); }
         color operator-(const color& other) const { return color(r - other.r, g - other.g, b - other.b, a); }

         // Comparison.
         bool operator == (const color& other) const
         {
            return r == other.r && g == other.g && b == other.b && a == other.a;
         }

         bool operator != (const color& other) const
         {
            return !(*this == other);
         }

         bool is_pale() const
         {
            return r >= 240 && g >= 240 && b >= 240;
         }

         bool is_dark() const
         {
            return r <= 20 && g <= 20 && b <= 20;
         }
      };

      class random_colors
      {
      public:
         color any();
         color red();
         color green();
         color blue();
         color cyan();
         color yellow();
         color magenta();
         color grey();

      private:
         color::channel next_color();

         std::minstd_rand rand;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
