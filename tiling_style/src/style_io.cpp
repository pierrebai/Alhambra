#include <dak/tiling_style/style_io.h>
#include <dak/tiling_style/styled_mosaic.h>

#include <dak/tiling_style/colored.h>
#include <dak/tiling_style/plain.h>
#include <dak/tiling_style/sketch.h>
#include <dak/tiling_style/filled.h>
#include <dak/tiling_style/thick.h>
#include <dak/tiling_style/outline.h>
#include <dak/tiling_style/emboss.h>
#include <dak/tiling_style/interlace.h>

#include <dak/tiling/figure.h>
#include <dak/tiling/star.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/irregular_figure.h>
#include <dak/tiling/extended_figure.h>
#include <dak/tiling/infer.h>

#include <dak/geometry/utility.h>

#include <iomanip>
#include <string>

namespace dak
{
   namespace tiling_style
   {
      using geometry::L;
      using tiling::infer_mode_from_name;
      using tiling::mosaic;

      namespace
      {
         ////////////////////////////////////////////////////////////////////////////
         //
         // Functions for reading and writing the styles.

         void read_colored(std::wistream& file, colored& new_style)
         {
            std::wstring dummy;
            int r = 0, g = 0, b = 0, a = 0;
            file >> dummy >> r >> g >> b >> a;
            new_style.color = ui::color(r, g, b, a);
         }

         void write_colored(std::wostream& file, const colored& style)
         {
            const int r = style.color.r, g = style.color.g, b = style.color.b, a = style.color.a;
            file << "  color " << r << " " << g << " " << b << " " << a << L"\n";
         }

         void read_plain(std::wistream& file, plain& new_style)
         {
            std::wstring dummy;
            file >> dummy;
            read_colored(file, new_style);
         }

         void write_plain(std::wostream& file, const plain& style)
         {
            file << L"  plain" << L"\n";
            write_colored(file, style);
         }

         void read_sketch(std::wistream& file, sketch& new_style)
         {
            std::wstring dummy;
            // Note: in the text format, sketch derives from plain... so we read two sentinels.
            file >> dummy >> dummy;
            read_colored(file, new_style);
         }

         void write_sketch(std::wostream& file, const sketch& style)
         {
            // Note: in the text format, sketch derives from plain... so we read two sentinels.
            file << L"  sktech" << L"\n"
                 << L"  plain" << L"\n";
            write_colored(file, style);
         }

         void read_filled(std::wistream& file, filled& new_style)
         {
            std::wstring dummy;
            file >> dummy >> std::boolalpha >> new_style.draw_inside >> std::boolalpha >> new_style.draw_outside;
            read_colored(file, new_style);
         }

         void write_filled(std::wostream& file, const filled& style)
         {
            file << L"  filled " << std::boolalpha << style.draw_inside << " " << std::boolalpha << style.draw_outside << L"\n";
            write_colored(file, style);
         }

         void read_thick(std::wistream& file, thick& new_style)
         {
            std::wstring dummy;
            file >> dummy >> new_style.width >> new_style.outline_width;
            read_colored(file, new_style);
         }

         void write_thick(std::wostream& file, const thick& style)
         {
            file << L"  thick " << style.width << " " << style.outline_width << L"\n";
            write_colored(file, style);
         }

         void read_outline(std::wistream& file, outline& new_style)
         {
            std::wstring dummy;
            file >> dummy;
            read_thick(file, new_style);
         }

         void write_outline(std::wostream& file, const outline& style)
         {
            file << L"  outline" << L"\n";
            write_thick(file, style);
         }

         void read_emboss(std::wistream& file, emboss& new_style)
         {
            std::wstring dummy;
            file >> dummy >> new_style.angle;
            read_outline(file, new_style);
         }

         void write_emboss(std::wostream& file, const emboss& style)
         {
            file << L"  emboss " << style.angle << L"\n";
            write_outline(file, style);
         }

         void read_interlace(std::wistream& file, interlace& new_style)
         {
            std::wstring dummy;
            file >> dummy >> new_style.gap_width >> new_style.shadow_width;
            // In the text format, interlace derives from thick, not outline.
            read_thick(file, new_style);
         }

         void write_interlace(std::wostream& file, const interlace& style)
         {
            file << L"  interlace " << style.gap_width << " " << style.shadow_width << L"\n";
            // In the text format, interlace derives from thick, not outline.
            write_thick(file, style);
         }

         void write_style(std::wostream& file, const style& a_style)
         {
            if (const auto interlace = dynamic_cast<const tiling_style::interlace *>(&a_style))
               write_interlace(file, *interlace);
            else if (const auto emboss = dynamic_cast<const tiling_style::emboss *>(&a_style))
               write_emboss(file, *emboss);
            else if (const auto outline = dynamic_cast<const tiling_style::outline *>(&a_style))
               write_outline(file, *outline);
            else if (const auto thick = dynamic_cast<const tiling_style::thick *>(&a_style))
               write_thick(file, *thick);
            else if (const auto filled = dynamic_cast<const tiling_style::filled *>(&a_style))
               write_filled(file, *filled);
            else if (const auto sketch = dynamic_cast<const tiling_style::sketch *>(&a_style))
               write_sketch(file, *sketch);
            else if (const auto plain = dynamic_cast<const tiling_style::plain *>(&a_style))
               write_plain(file, *plain);
            else
               throw std::exception(L::t("Unknown style type."));
         }

         ////////////////////////////////////////////////////////////////////////////
         //
         // Functions for reading and writing figures.

         void read_star(std::wistream& file, tiling::star& new_star)
         {
            std::wstring dummy;
            file >> dummy >> new_star.n >> new_star.d >> new_star.s;
         }

         void write_star(std::wostream& file, const tiling::star& star)
         {
            file << L"    star " << star.n << " " << star.d << " " << star.s << L"\n";
         }

         void read_rosette(std::wistream& file, tiling::rosette& new_rosette)
         {
            std::wstring dummy;
            file >> dummy >> new_rosette.n >> new_rosette.q >> new_rosette.s;
         }

         void write_rosette(std::wostream& file, const tiling::rosette& rosette)
         {
            file << L"    rosette " << rosette.n << " " << rosette.q << " " << rosette.s << L"\n";
         }

         void read_explicit_figure(std::wistream& file, tiling::irregular_figure&)
         {
            std::wstring dummy;
            file >> dummy;
         }

         void read_irregular_figure(std::wistream& file, tiling::irregular_figure& new_irregular)
         {
            std::wstring dummy;
            std::wstring infer;
            file >> dummy >> std::quoted(infer) >> new_irregular.q >> new_irregular.d >> new_irregular.s;
            new_irregular.infer = infer_mode_from_name(infer.c_str());
         }

         void write_irregular_figure(std::wostream& file, const tiling::irregular_figure& irregular)
         {
            file << L"    irregular " << std::quoted(infer_mode_name(irregular.infer)) << " " << irregular.q << " " << irregular.d << " " << irregular.s << L"\n";
         }

         void read_extended_figure(std::wistream& file, tiling::extended_figure& new_extended_figure)
         {
            std::wstring dummy;
            double dummy_s;
            tiling::rosette rosette;
            file >> dummy >> dummy_s >> rosette.n >> rosette.q >> rosette.s;
            // Note: for some reason, the mosaic saved from the old Taprats have bad Q and S values...
            if (rosette.q > 1.)
               rosette.q = 0.33;
            rosette.s = std::max(2, rosette.n / 3);
            new_extended_figure.n = rosette.n;
            new_extended_figure.child = std::make_shared<tiling::rosette>(rosette);
            new_extended_figure.child_changed();
         }

         void write_extended_figure(std::wostream& file, const tiling::extended_figure& extended_figure)
         {
            if (auto rosette = std::dynamic_pointer_cast<tiling::rosette>(extended_figure.child))
            {
               file << "    extended" << L"\n"
                    << "    " << 0 << " " << rosette->n << " " << rosette->q << " " << rosette->s << L"\n";
            }
            else
            {
               // TODO: extended without rosette.
            }
         }

         std::shared_ptr<mosaic> read_mosaic(std::wistream& file, const known_tilings& knowns)
         {
            auto new_mosaic = std::make_shared<mosaic>();

            std::wstring dummy;
            std::wstring tiling_name;
            int figure_count = 0;
            file >> dummy >> std::quoted(tiling_name) >> figure_count;

            for (const auto& tiling : knowns.tilings)
               if (tiling.name == tiling_name)
                  new_mosaic->tiling = tiling;

            for (int i = 0; i < figure_count; ++i)
            {
               std::wstring tile_type;
               int side_count = 0;
               file >> tile_type >> side_count;

               bool reg = (tile_type == L"regular");

               polygon poly;
               if (reg)
               {
                  poly = polygon::make_regular(side_count);
               }
               else
               {
                  for (int si = 0; si < side_count; ++si)
                  {
                     point pt;
                     file >> pt.x >> pt.y;
                     poly.points.emplace_back(pt);
                  }
               }
               std::shared_ptr<tiling::figure>& fig = new_mosaic->tile_figures[poly];

               std::wstring figure_type;
               const auto pos = file.tellg();
               file >> figure_type;
               file.seekg(pos);

               if (figure_type == L"star")
               {
                  std::shared_ptr<tiling::star> new_star(new tiling::star);
                  read_star(file, *new_star);
                  fig = new_star;
               }
               else if (figure_type == L"rosette")
               {
                  std::shared_ptr<tiling::rosette> new_rosette(new tiling::rosette);
                  read_rosette(file, *new_rosette);
                  fig = new_rosette;
               }
               else if (figure_type == L"explicit")
               {
                  std::shared_ptr<tiling::irregular_figure> new_irregular_figure(new tiling::irregular_figure(new_mosaic, poly));
                  read_explicit_figure(file, *new_irregular_figure);
                  fig = new_irregular_figure;
               }
               else if (figure_type == L"irregular")
               {
                  std::shared_ptr<tiling::irregular_figure> new_irregular_figure(new tiling::irregular_figure(new_mosaic, poly));
                  read_irregular_figure(file, *new_irregular_figure);
                  fig = new_irregular_figure;
               }
               else if (figure_type == L"extended")
               {
                  std::shared_ptr<tiling::radial_figure> child_rosette = std::make_shared<tiling::rosette>(6, 2, 2);
                  std::shared_ptr<tiling::extended_figure> new_extended_figure(new tiling::extended_figure(child_rosette));
                  read_extended_figure(file, *new_extended_figure);
                  fig = new_extended_figure;
               }
            }

            return new_mosaic;
         }

         void write_figure(std::wostream& file, const tiling::figure& fig)
         {
            if (const auto star = dynamic_cast<const tiling::star*>(&fig))
               write_star(file, *star);
            else if (const auto rosette = dynamic_cast<const tiling::rosette*>(&fig))
               write_rosette(file, *rosette);
            else if (const auto extended_figure = dynamic_cast<const tiling::extended_figure*>(&fig))
               write_extended_figure(file, *extended_figure);
            else if (const auto irregular_figure = dynamic_cast<const tiling::irregular_figure*>(&fig))
               write_irregular_figure(file, *irregular_figure);
            else
               throw std::exception(L::t("Unknown figure type."));
         }

         void write_mosaic(std::wostream& file, const mosaic& mosaic)
         {
            file << L"  mosaic " << std::quoted(mosaic.tiling.name) << " " << mosaic.tile_figures.size() << L"\n";

            for (const auto& poly_figure : mosaic.tile_figures)
            {
               const polygon& poly = poly_figure.first;
               const auto& fig = poly_figure.second;

               if (poly.is_regular())
               {
                  file << L"    regular " << poly.points.size() << L"\n";
               }
               else
               {
                  file << L"    polygon " << poly.points.size() << L"\n";
                  for (const auto& pt : poly.points)
                  {
                     file << "      " << pt.x << " " << pt.y << L"\n";
                  }
               }

               write_figure(file, *fig);
            }
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Functions for reading and writing a layered mosaic.

      std::vector<std::shared_ptr<ui::layer>> read_layered_mosaic(std::wistream& file, const known_tilings& knowns)
      {
         file.imbue(std::locale("C"));

         std::vector<std::shared_ptr<ui::layer>> new_layers;

         std::wstring dummy;
         size_t count = 0;
         file >> dummy >> count;

         for (size_t i = 0; i < count; ++i)
         {
            std::wstring style_type;
            const auto pos = file.tellg();
            file >> style_type;
            file.seekg(pos);

            std::shared_ptr<styled_mosaic> new_mosaic_layer(new styled_mosaic);
            if (style_type == L"emboss")
            {
               std::shared_ptr<emboss> new_emboss(new emboss);
               read_emboss(file, *new_emboss);
               new_mosaic_layer->style = new_emboss;
            }
            else if (style_type == L"filled")
            {
               std::shared_ptr<filled> new_filled(new filled);
               read_filled(file, *new_filled);
               new_mosaic_layer->style = new_filled;
            }
            else if (style_type == L"interlace")
            {
               std::shared_ptr<interlace> new_interlace(new interlace);
               read_interlace(file, *new_interlace);
               new_mosaic_layer->style = new_interlace;
            }
            else if (style_type == L"outline")
            {
               std::shared_ptr<outline> new_outline(new outline);
               read_outline(file, *new_outline);
               new_mosaic_layer->style = new_outline;
            }
            else if (style_type == L"plain")
            {
               std::shared_ptr<plain> new_plain(new plain);
               read_plain(file, *new_plain);
               new_mosaic_layer->style = new_plain;
            }
            else if (style_type == L"sketch")
            {
               std::shared_ptr<sketch> new_sketch(new sketch);
               read_sketch(file, *new_sketch);
               new_mosaic_layer->style = new_sketch;
            }
            else if (style_type == L"thick")
            {
               std::shared_ptr<thick> new_thick(new thick);
               read_thick(file, *new_thick);
               new_mosaic_layer->style = new_thick;
            }
            else
            {
               throw std::exception(L::t("Unknown style type."));
            }

            // TODO: shared identical mosaic between layers? That would speed-up map updates.
            new_mosaic_layer->mosaic = read_mosaic(file, knowns);

            new_layers.emplace_back(new_mosaic_layer);
         }

         return new_layers;
      }

      void write_layered_mosaic(std::wostream& file, const std::vector<std::shared_ptr<ui::layer>>& layers)
      {
         file.precision(17);
         file.imbue(std::locale("C"));

         file << L"layers " << layers.size() << "\n";

         for (const auto& layer : layers)
         {
            if (const auto styled_mosaic = std::dynamic_pointer_cast<tiling_style::styled_mosaic>(layer))
            {
               write_style(file, *styled_mosaic->style);
               write_mosaic(file, *styled_mosaic->mosaic);
            }
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
