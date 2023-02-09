#include <dak/tiling_style/mosaic_io.h>
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

#include <dak/utility/text.h>

#include <dak/geometry/geometry_io.h>

#include <iomanip>
#include <string>

namespace dak
{
   namespace tiling_style
   {
      using utility::L;
      using tiling::infer_mode_from_name;
      using tiling::mosaic_t;
      using geometry::transform_t;

      namespace
      {
         const wchar_t* colored_name   = L"color";
         const wchar_t* plain_name     = L"plain";
         const wchar_t* sketch_name    = L"sketch";
         const wchar_t* filled_name    = L"filled";
         const wchar_t* thick_name     = L"thick";
         const wchar_t* outline_name   = L"outline";
         const wchar_t* interlace_name = L"interlace";
         const wchar_t* emboss_name    = L"emboss";

         const wchar_t* old_layer_sentry = L"layers";
         const wchar_t* movable_layer_sentry = L"movable-layers";
         const wchar_t* movable_join_layer_sentry = L"movable-join-layers";
         const wchar_t* colored_movable_join_layer_sentry = L"colored-movable-join-layers";

         ////////////////////////////////////////////////////////////////////////////
         //
         // join-style streamer operator.

         std::wostream& operator<<(std::wostream& file, const stroke_t::join_style_t join_style)
         {
            const wchar_t* join_text = L"";
            switch (join_style)
            {
               case stroke_t::join_style_t::bevel: join_text = L"bevel"; break;
               case stroke_t::join_style_t::miter: join_text = L"miter"; break;
               case stroke_t::join_style_t::round: join_text = L"round"; break;
               default:                            join_text = L"round"; break;
            }

            file << join_text;
            return file;
         }
         
         std::wistream& operator>>(std::wistream& file, stroke_t::join_style_t& join_style)
         {
            std::wstring join_text;
            file >> join_text;

            if (join_text == L"miter")
               join_style = stroke_t::join_style_t::miter;
            else if (join_text == L"bevel")
               join_style = stroke_t::join_style_t::bevel;
            else
               join_style = stroke_t::join_style_t::round;

            return file;
         }

         ////////////////////////////////////////////////////////////////////////////
         //
         // Functions for reading and writing the styles.

         void read_colored(std::wistream& file, colored_t& new_style)
         {
            std::wstring dummy;
            int r = 0, g = 0, b = 0, a = 0;
            file >> dummy >> r >> g >> b >> a;
            new_style.color = ui::color_t(r, g, b, a);
         }

         void write_colored(std::wostream& file, const colored_t& style)
         {
            const int r = style.color.r, g = style.color.g, b = style.color.b, a = style.color.a;
            file << "  " << colored_name << " " << r << " " << g << " " << b << " " << a << L"\n";
         }

         void read_plain(std::wistream& file, plain_t& new_style)
         {
            std::wstring dummy;
            file >> dummy;
            read_colored(file, new_style);
         }

         void write_plain(std::wostream& file, const plain_t& style)
         {
            file << L"  " << plain_name << " " << L"\n";
            write_colored(file, style);
         }

         void read_sketch(std::wistream& file, sketch_t& new_style)
         {
            std::wstring dummy;
            // Note: in the text format, sketch derives from plain... so we read two sentinels.
            file >> dummy >> dummy;
            read_colored(file, new_style);
         }

         void write_sketch(std::wostream& file, const sketch_t& style)
         {
            // Note: in the text format, sketch derives from plain... so we read two sentinels.
            file << L"  " << sketch_name << L"\n"
                 << L"  " << plain_name << L"\n";
            write_colored(file, style);
         }

         void read_filled(std::wistream& file, filled_t& new_style)
         {
            std::wstring dummy;
            file >> dummy >> std::boolalpha >> new_style.draw_inside >> std::boolalpha >> new_style.draw_outside;
            read_colored(file, new_style);
         }

         void write_filled(std::wostream& file, const filled_t& style)
         {
            file << L"  " << filled_name << " " << std::boolalpha << style.draw_inside << " " << std::boolalpha << style.draw_outside << L"\n";
            write_colored(file, style);
         }

         void read_thick(std::wistream& file, thick_t& new_style, bool has_join, bool has_outline_color)
         {
            std::wstring dummy;
            file >> dummy >> new_style.width >> new_style.outline_width;
            if (has_join)
               file >> new_style.join;
            if (has_outline_color)
            {
               int r = 0, g = 0, b = 0, a = 0;
               file >> r >> g >> b >> a;
               new_style.outline_color = ui::color_t(r, g, b, a);
            }
            read_colored(file, new_style);
         }

         void write_thick(std::wostream& file, const thick_t& style)
         {
            file << L"  " << thick_name << " " << style.width << " " << style.outline_width << " " << style.join << L"\n";
            const int r = style.outline_color.r, g = style.outline_color.g, b = style.outline_color.b, a = style.outline_color.a;
            file << L"  " << r << " " << g << " " << b << " " << a << L"\n";
            write_colored(file, style);
         }

         void read_outline(std::wistream& file, outline_t& new_style, bool has_join, bool has_outline_color)
         {
            std::wstring dummy;
            file >> dummy;
            read_thick(file, new_style, has_join, has_outline_color);
         }

         void write_outline(std::wostream& file, const outline_t& style)
         {
            file << L"  " << outline_name << L"\n";
            write_thick(file, style);
         }

         void read_emboss(std::wistream& file, emboss_t& new_style, bool has_join, bool has_outline_color)
         {
            std::wstring dummy;
            file >> dummy >> new_style.angle;
            read_outline(file, new_style, has_join, has_outline_color);
         }

         void write_emboss(std::wostream& file, const emboss_t& style)
         {
            file << L"  " << emboss_name << " " << style.angle << L"\n";
            write_outline(file, style);
         }

         void read_interlace(std::wistream& file, interlace_t& new_style, bool has_join, bool has_outline_color)
         {
            std::wstring dummy;
            file >> dummy >> new_style.gap_width >> new_style.shadow_width;
            // In the text format, interlace derives from thick, not outline.
            read_thick(file, new_style, has_join, has_outline_color);
         }

         void write_interlace(std::wostream& file, const interlace_t& style)
         {
            file << L"  " << interlace_name << " " << style.gap_width << " " << style.shadow_width << L"\n";
            // In the text format, interlace derives from thick, not outline.
            write_thick(file, style);
         }

         void write_style(std::wostream& file, const style_t& a_style)
         {
            if (const auto interlace = dynamic_cast<const tiling_style::interlace_t *>(&a_style))
               write_interlace(file, *interlace);
            else if (const auto emboss = dynamic_cast<const tiling_style::emboss_t *>(&a_style))
               write_emboss(file, *emboss);
            else if (const auto outline = dynamic_cast<const tiling_style::outline_t *>(&a_style))
               write_outline(file, *outline);
            else if (const auto thick = dynamic_cast<const tiling_style::thick_t *>(&a_style))
               write_thick(file, *thick);
            else if (const auto filled = dynamic_cast<const tiling_style::filled_t *>(&a_style))
               write_filled(file, *filled);
            else if (const auto sketch = dynamic_cast<const tiling_style::sketch_t *>(&a_style))
               write_sketch(file, *sketch);
            else if (const auto plain = dynamic_cast<const tiling_style::plain_t *>(&a_style))
               write_plain(file, *plain);
            else
               throw std::exception(L::t("Unknown style type."));
         }

         ////////////////////////////////////////////////////////////////////////////
         //
         // Functions for reading and writing figures.

         void read_star(std::wistream& file, tiling::star_t& new_star)
         {
            std::wstring dummy;
            file >> dummy >> new_star.n >> new_star.d >> new_star.s;
         }

         void write_star(std::wostream& file, const tiling::star_t& star)
         {
            file << L"    star " << star.n << " " << star.d << " " << star.s << L"\n";
         }

         void read_rosette(std::wistream& file, tiling::rosette_t& new_rosette)
         {
            std::wstring dummy;
            file >> dummy >> new_rosette.n >> new_rosette.q >> new_rosette.s;
         }

         void write_rosette(std::wostream& file, const tiling::rosette_t& rosette)
         {
            file << L"    rosette " << rosette.n << " " << rosette.q << " " << rosette.s << L"\n";
         }

         void read_explicit_figure(std::wistream& file, tiling::irregular_figure_t&)
         {
            std::wstring dummy;
            file >> dummy;
         }

         void read_irregular_figure(std::wistream& file, tiling::irregular_figure_t& new_irregular)
         {
            std::wstring dummy;
            std::wstring infer;
            file >> dummy >> std::quoted(infer) >> new_irregular.q >> new_irregular.d >> new_irregular.s;
            new_irregular.infer = infer_mode_from_name(infer.c_str());
         }

         void write_irregular_figure(std::wostream& file, const tiling::irregular_figure_t& irregular)
         {
            file << L"    irregular " << std::quoted(infer_mode_name(irregular.infer)) << " " << irregular.q << " " << irregular.d << " " << irregular.s << L"\n";
         }

         void read_extended_figure(std::wistream& file, tiling::extended_figure_t& new_extended_figure)
         {
            std::wstring dummy;
            double dummy_s;
            tiling::rosette_t rosette;
            file >> dummy >> dummy_s >> rosette.n >> rosette.q >> rosette.s;
            // Note: for some reason, the mosaic saved from the old Taprats have bad Q and S values...
            if (rosette.q > 1.)
               rosette.q = 0.33;
            rosette.s = std::max(2, rosette.n / 3);
            new_extended_figure.n = rosette.n;
            new_extended_figure.child = std::make_shared<tiling::rosette_t>(rosette);
            new_extended_figure.child_changed();
         }

         void write_extended_figure(std::wostream& file, const tiling::extended_figure_t& extended_figure)
         {
            if (auto rosette = std::dynamic_pointer_cast<tiling::rosette_t>(extended_figure.child))
            {
               file << "    extended" << L"\n"
                    << "    " << 0 << " " << rosette->n << " " << rosette->q << " " << rosette->s << L"\n";
            }
            else
            {
               // TODO: extended without rosette.
            }
         }

         std::shared_ptr<mosaic_t> read_mosaic(std::wistream& file, const known_tilings_t& known_tilings)
         {
            auto new_mosaic = std::make_shared<mosaic_t>();

            std::wstring dummy;
            std::wstring tiling_name;
            int figure_count = 0;
            file >> dummy >> std::quoted(tiling_name) >> figure_count;

            new_mosaic->tiling = tiling::find_tiling(known_tilings, tiling_name);

            for (int i = 0; i < figure_count; ++i)
            {
               std::wstring tile_type;
               int side_count = 0;
               file >> tile_type >> side_count;

               bool reg = (tile_type == L"regular");

               polygon_t poly;
               if (reg)
               {
                  poly = polygon_t::make_regular(side_count);
               }
               else
               {
                  for (int si = 0; si < side_count; ++si)
                  {
                     point_t pt;
                     file >> pt.x >> pt.y;
                     poly.points.emplace_back(pt);
                  }
               }
               std::shared_ptr<tiling::figure_t>& fig = new_mosaic->tile_figures[poly];

               std::wstring figure_type;
               const auto pos = file.tellg();
               file >> figure_type;
               file.seekg(pos);

               if (figure_type == L"star")
               {
                  std::shared_ptr<tiling::star_t> new_star(new tiling::star_t);
                  read_star(file, *new_star);
                  fig = new_star;
               }
               else if (figure_type == L"rosette")
               {
                  std::shared_ptr<tiling::rosette_t> new_rosette(new tiling::rosette_t);
                  read_rosette(file, *new_rosette);
                  fig = new_rosette;
               }
               else if (figure_type == L"explicit")
               {
                  std::shared_ptr<tiling::irregular_figure_t> new_irregular_figure(new tiling::irregular_figure_t(new_mosaic, poly));
                  read_explicit_figure(file, *new_irregular_figure);
                  fig = new_irregular_figure;
               }
               else if (figure_type == L"irregular")
               {
                  std::shared_ptr<tiling::irregular_figure_t> new_irregular_figure(new tiling::irregular_figure_t(new_mosaic, poly));
                  read_irregular_figure(file, *new_irregular_figure);
                  fig = new_irregular_figure;
               }
               else if (figure_type == L"extended")
               {
                  std::shared_ptr<tiling::radial_figure_t> child_rosette = std::make_shared<tiling::rosette_t>(6, 2, 2);
                  std::shared_ptr<tiling::extended_figure_t> new_extended_figure(new tiling::extended_figure_t(child_rosette));
                  read_extended_figure(file, *new_extended_figure);
                  fig = new_extended_figure;
               }
            }

            return new_mosaic;
         }

         void write_figure(std::wostream& file, const tiling::figure_t& fig)
         {
            if (const auto star = dynamic_cast<const tiling::star_t*>(&fig))
               write_star(file, *star);
            else if (const auto rosette = dynamic_cast<const tiling::rosette_t*>(&fig))
               write_rosette(file, *rosette);
            else if (const auto extended_figure = dynamic_cast<const tiling::extended_figure_t*>(&fig))
               write_extended_figure(file, *extended_figure);
            else if (const auto irregular_figure = dynamic_cast<const tiling::irregular_figure_t*>(&fig))
               write_irregular_figure(file, *irregular_figure);
            else
               throw std::exception(L::t("Unknown figure type."));
         }

         void write_mosaic(std::wostream& file, const mosaic_t& mosaic)
         {
            file << L"  mosaic " << std::quoted(mosaic.tiling->name) << " " << mosaic.tile_figures.size() << L"\n";

            for (const auto& poly_figure : mosaic.tile_figures)
            {
               const polygon_t& poly = poly_figure.first;
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


         transform_t read_transform(std::wistream& file)
         {
            transform_t trf;
            std::wstring dummy;
            file >> dummy >> trf;
            return trf;
         }

         void write_transform(std::wostream& file, const transform_t& trf)
         {
            file << L"  position " << trf << L"\n";
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Functions for reading and writing a layered mosaic.

      std::vector<std::shared_ptr<styled_mosaic_t>> read_layered_mosaic(std::wistream& file, const known_tilings_t& known_tilings)
      {
         file.imbue(std::locale("C"));

         std::vector<std::shared_ptr<styled_mosaic_t>> new_layers;

         std::wstring sentry;
         size_t count = 0;
         file >> sentry >> count;

         const bool is_colored = (sentry == colored_movable_join_layer_sentry);
         const bool is_movable_join = (sentry == movable_join_layer_sentry || sentry == colored_movable_join_layer_sentry);
         const bool is_movable = (is_movable_join || sentry == movable_layer_sentry || sentry == colored_movable_join_layer_sentry);

         for (size_t i = 0; i < count; ++i)
         {
            std::wstring style_type;
            const auto pos = file.tellg();
            file >> style_type;
            file.seekg(pos);

            std::shared_ptr<styled_mosaic_t> new_mosaic_layer(new styled_mosaic_t);
            if (style_type == emboss_name)
            {
               std::shared_ptr<emboss_t> new_emboss(new emboss_t);
               read_emboss(file, *new_emboss, is_movable_join, is_colored);
               new_mosaic_layer->style = new_emboss;
            }
            else if (style_type == filled_name)
            {
               std::shared_ptr<filled_t> new_filled(new filled_t);
               read_filled(file, *new_filled);
               new_mosaic_layer->style = new_filled;
            }
            else if (style_type == interlace_name)
            {
               std::shared_ptr<interlace_t> new_interlace(new interlace_t);
               read_interlace(file, *new_interlace, is_movable_join, is_colored);
               new_mosaic_layer->style = new_interlace;
            }
            else if (style_type == outline_name)
            {
               std::shared_ptr<outline_t> new_outline(new outline_t);
               read_outline(file, *new_outline, is_movable_join, is_colored);
               new_mosaic_layer->style = new_outline;
            }
            else if (style_type == plain_name)
            {
               std::shared_ptr<plain_t> new_plain(new plain_t);
               read_plain(file, *new_plain);
               new_mosaic_layer->style = new_plain;
            }
            else if (style_type == sketch_name)
            {
               std::shared_ptr<sketch_t> new_sketch(new sketch_t);
               read_sketch(file, *new_sketch);
               new_mosaic_layer->style = new_sketch;
            }
            else if (style_type == thick_name)
            {
               std::shared_ptr<thick_t> new_thick(new thick_t);
               read_thick(file, *new_thick, is_movable_join, is_colored);
               new_mosaic_layer->style = new_thick;
            }
            else
            {
               throw std::exception(L::t("Unknown style type."));
            }

            // TODO: shared identical mosaic between layers? That would speed-up map updates.
            new_mosaic_layer->mosaic = read_mosaic(file, known_tilings);

            if (is_movable)
               new_mosaic_layer->set_transform(read_transform(file));

            new_layers.emplace_back(new_mosaic_layer);
         }

         return new_layers;
      }

      void write_layered_mosaic(std::wostream& file, const std::vector<std::shared_ptr<styled_mosaic_t>>& layers)
      {
         file.precision(17);
         file.imbue(std::locale("C"));

         file << colored_movable_join_layer_sentry << L"  " << layers.size() << "\n";

         for (const auto& styled_mosaic : layers)
         {
            write_style(file, *styled_mosaic->style);
            write_mosaic(file, *styled_mosaic->mosaic);
            write_transform(file, styled_mosaic->get_transform());
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
