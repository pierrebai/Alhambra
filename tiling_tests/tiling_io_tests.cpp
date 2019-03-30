#include <dak/tiling/known_tilings.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/star.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/face.h>
#include <dak/geometry/point_utility.h>

#include <filesystem>
#include <fstream>
#include <iomanip>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace dak::geometry;
using namespace dak::tiling;
using namespace std::experimental::filesystem;

namespace tiling_tests
{		
	TEST_CLASS(tiling_io_tests)
	{
	public:
		
		TEST_METHOD(tiling_io_read_all_tilings)
		{
         std::vector<std::wstring> errors;
         known_tilings tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings.tilings)
         {
            Assert::IsFalse(tiling.name.empty());
            Assert::IsFalse(tiling.description.empty());
            Assert::IsFalse(tiling.author.empty());
            Assert::IsFalse(tiling.tiles.empty());
         }
      }

      //#define SLOW_DAK_GEOMETRY_TILING_IO_TESTS

      #ifdef SLOW_DAK_GEOMETRY_TILING_IO_TESTS

      TEST_METHOD(tiling_io_draw_all_tilings)
      {
         path drawings_dir = path(KNOWN_TILINGS_DIR) / path(L"..") / path(L"drawings");
         create_directory(drawings_dir);

         std::vector<std::wstring> errors;
         known_tilings tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings.tilings)
         {
            // Create a mosaic with the tiling.
            auto mo = std::make_shared<mosaic>(tiling);

            // Fill all regular tiles with rosette.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon& tile = placed.first;
               if (tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<rosette>(int(tile.points.size()), 0.1, int(tile.points.size()) / 4);
               }
            }

            // Fill all irregular tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon& tile = placed.first;
               if (!tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<irregular_figure>(mo, tile);
               }
            }

            std::wofstream drawing_file(drawings_dir / (tiling.name + L".csv"), std::ios::trunc | std::ios::out);
            rect region(point(0, 0), point(30, 30));
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &drawing_file](int t1, int t2) {
               const transform receive_trf = transform::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto placed_tile : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(placed_tile.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : placed_tile.second)
                  {
                     //point p1 = placed_tile.first->points.back();
                     //p1 = p1.apply(trf).apply(receive_trf);
                     //for (point p2 : placed_tile.first->points)
                     //{
                     //   p2 = p2.apply(trf).apply(receive_trf);
                     //   file << p1.x << L", " << p1.y << L", " << p2.x << L", " << p2.y << L"\n";
                     //   p1 = p2;
                     //}
                     for (const auto& edge : map.canonicals())
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
         known_tilings tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings.tilings)
         {
            if (counter++ % 10 != 0)
               continue;

            // Create a mosaic with the tiling.
            auto mo = std::make_shared<mosaic>(tiling);

            // Fill all regular tiles with rosette.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon& tile = placed.first;
               if (tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<rosette>(int(tile.points.size()), 0.1, int(tile.points.size()) / 4);
               }
            }

            // Fill all irregular tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon& tile = placed.first;
               if (!tile.is_regular())
               {
                  mo->tile_figures[tile] = std::make_shared<irregular_figure>(mo, tile);
               }
            }

            std::wofstream drawing_file(drawings_dir / (tiling.name + L".csv"), std::ios::trunc | std::ios::out);
            rect region(point(0, 0), point(30, 30));
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &drawing_file](int t1, int t2) {
               const transform receive_trf = transform::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto placed_tile : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(placed_tile.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : placed_tile.second)
                  {
                     //point p1 = placed_tile.first->points.back();
                     //p1 = p1.apply(trf).apply(receive_trf);
                     //for (point p2 : placed_tile.first->points)
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
         known_tilings tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings.tilings)
         {
            // Create a mosaic with the tiling.
            auto mo = std::make_shared<mosaic>(tiling);

            // Fill all tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon& tile = placed.first;
               mo->tile_figures[tile]= std::make_shared<irregular_figure>(mo, tile);
            }

            map final_map;
            rect region(point(0, 0), point(30, 30));
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &final_map](int t1, int t2) {
               const transform receive_trf = transform::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto placed_tile : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(placed_tile.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : placed_tile.second)
                  {
                     const auto placed = map.apply(trf).apply(receive_trf);
                     final_map.merge_non_overlapping(placed);
                  }
               }
            });

            const auto errors = final_map.verify();
            Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");

            face::faces white, black, red, exteriors;
            face::make_faces(final_map, white, black, red, exteriors);

            const auto face_errors = face::verify(final_map, white, black, red, exteriors);
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
         known_tilings tilings(KNOWN_TILINGS_DIR, errors);
         for (const auto& tiling : tilings.tilings)
         {
            if (counter++ % 10 != 0)
               continue;

            // Create a mosaic with the tiling.
            auto mo = std::make_shared<mosaic>(tiling);

            // Fill all tiles with inferred girih.
            for (const auto& placed : mo->tiling.tiles)
            {
               const polygon& tile = placed.first;
               mo->tile_figures[tile] = std::make_shared<irregular_figure>(mo, tile);
            }

            map final_map;
            rect region(point(0, 0), point(30, 30));
            fill(region, mo->tiling.t1, mo->tiling.t2, [&mo, &final_map](int t1, int t2) {
               const transform receive_trf = transform::translate(mo->tiling.t1.scale(t1) + mo->tiling.t2.scale(t2));
               for (const auto placed_tile : mo->tiling.tiles)
               {
                  const auto iter = mo->tile_figures.find(placed_tile.first);
                  if (iter == mo->tile_figures.end())
                     continue;
                  auto map = iter->second->get_map();
                  const auto errors = map.verify();
                  Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");
                  for (const auto& trf : placed_tile.second)
                  {
                     const auto placed = map.apply(trf).apply(receive_trf);
                     final_map.merge_non_overlapping(placed);
                  }
               }
            });

            const auto errors = final_map.verify();
            Assert::AreEqual<size_t>(0, errors.size(), errors.size() > 0 ? errors[0].c_str() : L"no error");

            face::faces white, black, red, exteriors;
            face::make_faces(final_map, white, black, red, exteriors);

            const auto face_errors = face::verify(final_map, white, black, red, exteriors);
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
   };
}