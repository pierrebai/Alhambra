#include <dak/tiling/tiling_io.h>

#include <dak/tiling/translation_tiling.h>
#include <dak/tiling/inflation_tiling.h>

#include <dak/utility/text.h>

#include <dak/geometry/geometry_io.h>

#include <iomanip>
#include <map>

namespace dak
{
   namespace tiling
   {
      using utility::L;

      static const wchar_t translation_tiling_sentry[] = L"tiling";
      static const wchar_t inflation_tiling_sentry[] = L"inflation-tiling-2";
      static const wchar_t inflation_old_tiling_sentry[] = L"inflation-tiling";

      ////////////////////////////////////////////////////////////////////////////
      //
      // tiling I/O.
      //
      // Functions for reading tilings in from files.  No real error-checking
      // is done, because I don't expect you to write these files by hand --
      // they should be auto-generated from DesignerPanel.

      static void read_tiles(std::wistream& file, tiling_t& new_tiling, int tile_count)
      {
         for (int i = 0; i < tile_count; ++i)
         {
            std::wstring tile_type;
            int side_count = 0;
            int trf_count = 0;
            file >> tile_type >> side_count >> trf_count;

            bool reg = (tile_type == L"regular");

            std::vector<transform_t>* trfs = nullptr;
            if (reg)
            {
               trfs = &new_tiling.tiles[polygon_t::make_regular(side_count)];
            }
            else
            {
               polygon_t poly;
               for (int si = 0; si < side_count; ++si)
               {
                  point_t pt;
                  file >> pt.x >> pt.y;
                  poly.points.emplace_back(pt);
               }
               trfs = &new_tiling.tiles[poly];
            }

            for (int trfi = 0; trfi < trf_count; ++trfi)
            {
               transform_t trf;
               file >> trf.scale_x >> trf.rot_1 >> trf.trans_x
                  >> trf.rot_2 >> trf.scale_y >> trf.trans_y;
               trfs->emplace_back(trf);
            }
         }
      }

      static void read_descriptions(std::wistream& file, tiling_t& new_tiling)
      {
         file >> std::quoted(new_tiling.description) >> quoted(new_tiling.author);
      }

      std::shared_ptr<tiling_t> read_tiling(std::wistream& file)
      {
         file.imbue(std::locale("C"));

         std::wstring sentry;
         file >> sentry;
         if (sentry != translation_tiling_sentry && sentry != inflation_tiling_sentry && sentry != inflation_old_tiling_sentry)
            throw std::exception(L::t("This isn't a tiling file."));

         const bool is_inflation = (sentry == inflation_tiling_sentry);
         const bool is_old_inflation = (sentry == inflation_old_tiling_sentry);

         std::wstring name;
         file >> std::quoted(name);
         if (name.empty())
            throw std::exception(L::t("Invalid tiling file."));

         int tile_count = 0;
         file >> tile_count;

         std::shared_ptr<tiling_t> new_tiling;

         if (is_inflation)
         {
            edge_t s1;
            edge_t s2;
            transform_t inflation;

            file >> s1.p1.x >> s1.p1.y >> s1.p2.x >> s1.p2.y;
            file >> s2.p1.x >> s2.p1.y >> s2.p2.x >> s2.p2.y;

            s1.order = s1.angle();
            s2.order = s2.angle();

            file >> inflation;

            new_tiling = std::make_shared<inflation_tiling_t>(name, s1, s2, inflation);
         }
         else if (is_old_inflation)
         {
            edge_t s1;
            edge_t s2;
            double factor;

            file >> s1.p1.x >> s1.p1.y >> s1.p2.x >> s1.p2.y;
            file >> s2.p1.x >> s2.p1.y >> s2.p2.x >> s2.p2.y;

            s1.order = s1.angle();
            s2.order = s2.angle();

            file >> factor;

            transform_t inflation = transform_t::scale(factor);

            new_tiling = std::make_shared<inflation_tiling_t>(name, s1, s2, inflation);
         }
         else
         {
            point_t t1;
            point_t t2;
            file >> t1.x >> t1.y >> t2.x >> t2.y;

            new_tiling = std::make_shared<translation_tiling_t>(name, t1, t2);
         }

         read_tiles(file, *new_tiling, tile_count);
         read_descriptions(file, *new_tiling);

         return new_tiling;
      }

      void write_tiling(const std::shared_ptr<const tiling_t>& tiling, std::wostream& file)
      {
         file.precision(17);
         file.imbue(std::locale("C"));

         if (auto trans_tiling = std::dynamic_pointer_cast<const translation_tiling_t>(tiling))
         {
            file << translation_tiling_sentry << L" " << std::quoted(trans_tiling->name) << L" " << trans_tiling->tiles.size() << std::endl;
            file << L"    " << trans_tiling->t1.x << L" " << trans_tiling->t1.y << std::endl;
            file << L"    " << trans_tiling->t2.x << L" " << trans_tiling->t2.y << std::endl;
            file << std::endl;

         }
         else if (auto inflation_tiling = std::dynamic_pointer_cast<const inflation_tiling_t>(tiling))
         {
            file << inflation_tiling_sentry << L" " << std::quoted(inflation_tiling->name) << L" " << inflation_tiling->tiles.size() << std::endl;
            file << L"    " << inflation_tiling->s1.p1.x << L" " << inflation_tiling->s1.p1.y << std::endl;
            file << L"    " << inflation_tiling->s1.p2.x << L" " << inflation_tiling->s1.p2.y << std::endl;
            file << L"    " << inflation_tiling->s2.p1.x << L" " << inflation_tiling->s2.p1.y << std::endl;
            file << L"    " << inflation_tiling->s2.p2.x << L" " << inflation_tiling->s2.p2.y << std::endl;
            file << L"    " << inflation_tiling->inflation  << std::endl;
            file << std::endl;
         }

         for (const auto& poly_trf : tiling->tiles)
         {
            const polygon_t& poly = poly_trf.first;
            const std::vector<transform_t>& trfs = poly_trf.second;

            if (poly.is_regular())
            {
               file << L"        regular " << poly.points.size() << L" " << trfs.size() << std::endl;
            }
            else {
               file << L"        polygon " << poly.points.size() << L" " << trfs.size() << std::endl;
               for (const point_t& pt : poly.points)
                  file << L"        " << pt.x << L" " << pt.y << std::endl;
            }
            file << std::endl;

            for (const auto& trf : trfs)
            {
               file << L"            " << trf.scale_x << L" " << trf.rot_1   << L" " << trf.trans_x
                               << L" " << trf.rot_2   << L" " << trf.scale_y << L" " << trf.trans_y << std::endl;
            }
            file << std::endl;
         }

         file << L"   " << std::quoted(tiling->description) << std::endl;
         file << L"   " << std::quoted(tiling->author) << std::endl;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
