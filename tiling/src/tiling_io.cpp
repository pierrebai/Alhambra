#include <dak/tiling/tiling_io.h>

#include <dak/tiling/translation_tiling.h>

#include <dak/utility/text.h>

#include <iomanip>
#include <map>

namespace dak
{
   namespace tiling
   {
      using utility::L;

      ////////////////////////////////////////////////////////////////////////////
      //
      // tiling I/O.
      //
      // Functions for reading tilings in from files.  No real error-checking
      // is done, because I don't expect you to write these files by hand --
      // they should be auto-generated from DesignerPanel.

      std::shared_ptr<tiling_t> read_tiling(std::wistream& file)
      {
         file.imbue(std::locale("C"));

         std::wstring sentry;
         file >> sentry;
         if (sentry != L"tiling")
            throw std::exception(L::t("This isn't a tiling file."));

         std::wstring name;
         file >> std::quoted(name);
         if (name.empty())
            throw std::exception(L::t("Invalid tiling file."));

         int tile_count = 0;
         file >> tile_count;

         point_t t1;
         point_t t2;
         file >> t1.x >> t1.y >> t2.x >> t2.y;

         translation_tiling_t new_tiling(name, t1, t2);

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
               file >> trf.scale_x >> trf.rot_1   >> trf.trans_x
                    >> trf.rot_2   >> trf.scale_y >> trf.trans_y;
               trfs->emplace_back(trf);
            }
         }

         file >> std::quoted(new_tiling.description) >> quoted(new_tiling.author);

         return std::make_shared<translation_tiling_t>(new_tiling);
      }

      void write_tiling(const std::shared_ptr<const tiling_t>& tiling, std::wostream& file)
      {
         auto trans_tiling = std::dynamic_pointer_cast<const translation_tiling_t>(tiling);
         if (!trans_tiling)
            return;

         file.precision(17);
         file.imbue(std::locale("C"));

         file << L"tiling " << std::quoted(trans_tiling->name) << L" " << trans_tiling->tiles.size() << std::endl;
         file << L"    " << trans_tiling->t1.x << L" " << trans_tiling->t1.y << std::endl;
         file << L"    " << trans_tiling->t2.x << L" " << trans_tiling->t2.y << std::endl;
         file << std::endl;

         for (const auto& poly_trf : trans_tiling->tiles)
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

         file << L"   " << std::quoted(trans_tiling->description) << std::endl;
         file << L"   " << std::quoted(trans_tiling->author) << std::endl;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
