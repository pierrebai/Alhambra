#include <dak/tiling/known_tilings.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/star.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/face.h>
#include <dak/geometry/utility.h>

#include <filesystem>
#include <fstream>
#include <iomanip>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;
using namespace dak::tiling;
using namespace std::filesystem;

namespace tiling_tests
{		
	TEST_CLASS(tiling_io_tests)
	{
	public:
      #define KNOWN_TILINGS_DIR L""
		
		TEST_METHOD(tiling_io_read_all_tilings)
		{
         std::vector<std::wstring> errors;
         known_tilings_t tilings = read_tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings)
         {
            Assert::IsFalse(tiling.name.empty());
            Assert::IsFalse(tiling.description.empty());
            Assert::IsFalse(tiling.author.empty());
            Assert::IsFalse(tiling.tiles.empty());
         }
      }

      #define SLOW_DAK_GEOMETRY_TILING_IO_TESTS

      #ifdef SLOW_DAK_GEOMETRY_TILING_IO_TESTS

      TEST_METHOD(tiling_io_draw_all_tilings)
      {
         path drawings_dir = path(KNOWN_TILINGS_DIR) / path(L"..") / path(L"drawings");
         create_directory(drawings_dir);

         std::vector<std::wstring> errors;
         known_tilings_t tilings = read_tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings)
         {
            // Create a mosaic_t with the tiling.
            auto mo = std::make_shared<mosaic_t>(tiling);

            // Fill all regular tiles with rosette_t.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon_t& tile = placed.first;
               if (tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<rosette_t>(int(tile.points.size()), 0.1, int(tile.points.size()) / 4);
               }
            }

            // Fill all irregular tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon_t& tile = placed.first;
               if (!tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<irregular_figure_t>(mo, tile);
               }
            }

            std::wofstream drawing_file(drawings_dir / (tiling.name + L".csv"), std::ios::trunc | std::ios::out);
            rectangle_t region(point_t(0, 0), point_t(30, 30));
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &drawing_file](int t1, int t2) {
               const transform_t receive_trf = transform_t::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto tile_placements : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(tile_placements.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : tile_placements.second)
                  {
                     //point_t p1 = tile_placements.first->points.back();
                     //p1 = p1.apply(trf).apply(receive_trf);
                     //for (point_t p2 : tile_placements.first->points)
                     //{
                     //   p2 = p2.apply(trf).apply(receive_trf);
                     //   file << p1.x << L", " << p1.y << L", " << p2.x << L", " << p2.y << L"\n";
                     //   p1 = p2;
                     //}
                     for (const auto& edge : map.all())
                     {
                        const auto placed = edge.apply(trf).apply(receive_trf);
                        drawing_file << placed.p1.x << L", " << placed.p1.y << L", " << placed.p2.x << L", " << placed.p2.y << L"\n";
                     }
                  }
               }
            });
         }
      }

      #endif

      TEST_METHOD(tiling_io_draw_some_tilings)
      {
         path drawings_dir = path(KNOWN_TILINGS_DIR) / path(L"..") / path(L"drawings");
         create_directory(drawings_dir);

         int counter = 0;

         std::vector<std::wstring> errors;
         known_tilings_t tilings = read_tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings)
         {
            if (counter++ % 10 != 0)
               continue;

            // Create a mosaic_t with the tiling.
            auto mo = std::make_shared<mosaic_t>(tiling);

            // Fill all regular tiles with rosette_t.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon_t& tile = placed.first;
               if (tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<rosette_t>(int(tile.points.size()), 0.1, int(tile.points.size()) / 4);
               }
            }

            // Fill all irregular tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon_t& tile = placed.first;
               if (!tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<irregular_figure_t>(mo, tile);
               }
            }

            std::wofstream drawing_file(drawings_dir / (tiling.name + L".csv"), std::ios::trunc | std::ios::out);
            rectangle_t region(point_t(0, 0), point_t(30, 30));
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &drawing_file](int t1, int t2) {
               const transform_t receive_trf = transform_t::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto tile_placements : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(tile_placements.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : tile_placements.second)
                  {
                     //point_t p1 = tile_placements.first->points.back();
                     //p1 = p1.apply(trf).apply(receive_trf);
                     //for (point_t p2 : tile_placements.first->points)
                     //{
                     //   p2 = p2.apply(trf).apply(receive_trf);
                     //   file << p1.x << L", " << p1.y << L", " << p2.x << L", " << p2.y << L"\n";
                     //   p1 = p2;
                     //}
                     for (const auto& edge : map.all())
                     {
                        if (!edge.is_canonical())
                           continue;
                        const auto placed = edge.apply(trf).apply(receive_trf);
                        drawing_file << placed.p1.x << L", " << placed.p1.y << L", " << placed.p2.x << L", " << placed.p2.y << L"\n";
                     }
                  }
               }
            });
         }
      }

      #ifdef SLOW_DAK_GEOMETRY_TILING_IO_TESTS

      TEST_METHOD(tiling_io_draw_all_tilings_faces)
      {
         path drawings_dir = path(KNOWN_TILINGS_DIR) / path(L"..") / path(L"drawings");
         create_directory(drawings_dir);

         std::vector<std::wstring> errors;
         known_tilings_t tilings = read_tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings)
         {
            // Create a mosaic_t with the tiling.
            auto mo = std::make_shared<mosaic_t>(tiling);

            // Fill all tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon_t& tile = placed.first;
               mo->tile_figures[tile]= std::make_shared<irregular_figure_t>(mo, tile);
            }

            edges_map_t final_map;
            rectangle_t region(point_t(0, 0), point_t(30, 30));
            final_map.reserve(count_fill_replications(region, mo->tiling.t1, mo->tiling.t2) * mo->count_tiling_edges());
            final_map.begin_merge_non_overlapping();
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &final_map](int t1, int t2) {
               const transform_t receive_trf = transform_t::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto tile_placements : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(tile_placements.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : tile_placements.second)
                  {
                     const auto placed = map.apply(trf).apply(receive_trf);
                     final_map.merge_non_overlapping(placed);
                  }
               }
            });
            final_map.end_merge_non_overlapping();

            const auto errors = final_map.verify();
            Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");

            face_t::faces_t white, black, red, exteriors;
            face_t::make_faces(final_map, white, black, red, exteriors);

            const auto face_errors = face_t::verify(final_map, white, black, red, exteriors);
            Assert::AreEqual<size_t>(0, face_errors.size(), face_errors.size() > 0 ? face_errors[0].c_str() : L"no error");

            std::wofstream drawing_file(drawings_dir / (tiling.name + L".faces.csv"), std::ios::trunc | std::ios::out);
            for (const auto& face : white)
            {
               for (const auto& pt : face.points)
               {
                  drawing_file << pt.x << L", " << pt.y << L"\n";
               }
            }
            for (const auto& face : black)
            {
               for (const auto& pt : face.points)
               {
                  drawing_file << pt.x << L", " << pt.y << L"\n";
               }
            }
         }
      }

      #endif

      TEST_METHOD(tiling_io_draw_some_tilings_faces)
      {
         path drawings_dir = path(KNOWN_TILINGS_DIR) / path(L"..") / path(L"drawings");
         create_directory(drawings_dir);

         int counter = 0;

         std::vector<std::wstring> errors;
         known_tilings_t tilings = read_tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings)
         {
            if (counter++ % 10 != 0)
               continue;

            // Create a mosaic_t with the tiling.
            auto mo = std::make_shared<mosaic_t>(tiling);

            // Fill all tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon_t& tile = placed.first;
               mo->tile_figures[tile] = std::make_shared<irregular_figure_t>(mo, tile);
            }

            edges_map_t final_map;
            rectangle_t region(point_t(0, 0), point_t(30, 30));
            final_map.reserve(count_fill_replications(region, mo->tiling.t1, mo->tiling.t2) * mo->count_tiling_edges());
            final_map.begin_merge_non_overlapping();
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &final_map](int t1, int t2) {
               const transform_t receive_trf = transform_t::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto tile_placements : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(tile_placements.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : tile_placements.second)
                  {
                     const auto placed = map.apply(trf).apply(receive_trf);
                     final_map.merge_non_overlapping(placed);
                  }
               }
            });
            final_map.end_merge_non_overlapping();

            const auto errors = final_map.verify();
            Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? (tiling.name + std::wstring(L": ") + errors[0]).c_str() : L"no error");

            face_t::faces_t white, black, red, exteriors;
            face_t::make_faces(final_map, white, black, red, exteriors);

            const auto face_errors = face_t::verify(final_map, white, black, red, exteriors);
            Assert::AreEqual<size_t>(0, face_errors.size(), face_errors.size() > 0 ? (tiling.name + std::wstring(L": ") + face_errors[0]).c_str() : L"no error");

            std::wofstream drawing_file(drawings_dir / (tiling.name + L".faces.csv"), std::ios::trunc | std::ios::out);
            for (const auto& face : white)
            {
               for (const auto& pt : face.points)
               {
                  drawing_file << pt.x << L", " << pt.y << L"\n";
               }
            }
            for (const auto& face : black)
            {
               for (const auto& pt : face.points)
               {
                  drawing_file << pt.x << L", " << pt.y << L"\n";
               }
            }
         }
      }
   };
}
