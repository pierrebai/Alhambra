#include <dak/geometry/face.h>
#include <dak/utility/text.h>

#include <algorithm>
#include <numeric>
#include <deque>
#include <map>

namespace dak
{
   namespace geometry
   {
      using utility::L;

      namespace
      {
         //#define DAK_GEOMETRY_FACE_FACES_INTERNAL_VERIFY

         void automatic_verify(const map& map, const face::faces& white, const face::faces& black, const face::faces& red, const face::faces& exteriors)
         {
            #ifdef DAK_GEOMETRY_FACE_FACES_INTERNAL_VERIFY
               face::verify_and_throw(map, white, black, red, exteriors);
            #endif
         }

         std::vector<std::wstring> internal_verify(const map& map, const face::faces& white, const face::faces& black, const face::faces& red, const face::faces& exteriors)
         {
            std::vector<std::wstring> errors;

            // Unfortunately, some tilings are degenrate with faces that are simple lines and cause checks to fail.
            // Don't verify those tilings.
            const std::vector<polygon>* all_faces[4] = { &white, &black, &red, &exteriors };
            for (const auto& faces : all_faces)
            {
               for (const polygon& face : *faces)
               {
                  if (face.points.size() < 3)
                  {
                     return errors;
                  }
               }
            }
            
            wchar_t error[200];

            // The number of white and black face times their number of edges should be equal.
            const size_t white_edges = std::accumulate(white.begin(), white.end(), size_t(0), [](size_t val, const polygon& f) { return f.points.size() + val; });
            const size_t black_edges = std::accumulate(black.begin(), black.end(), size_t(0), [](size_t val, const polygon& f) { return f.points.size() + val; });
            const size_t red_edges = std::accumulate(red.begin(), red.end(), size_t(0), [](size_t val, const polygon& f) { return f.points.size() + val; });
            const size_t exterior_edges = std::accumulate(exteriors.begin(), exteriors.end(), size_t(0), [](size_t val, const polygon& f) { return f.points.size() + val; });
            if (std::abs(long(white_edges - black_edges)) > red_edges + exterior_edges)
            {
               _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"White faces contains more edges than black faces (%ld vs %ld vs %ld vs %ld)."),
                              long(white_edges), long(black_edges), long(red_edges), long(exterior_edges));
               errors.emplace_back(error);
            }

            // Make sure there are exactly as many points in the faces as in the map.
            const size_t map_edge_count = map.all().size();
            const size_t faces_edge_count = white_edges + black_edges + red_edges + exterior_edges;
            if (map_edge_count != faces_edge_count)
            {
               _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Face and map do not contain the same number of points: %ld vs. %ld."),
                              long(faces_edge_count), long(map_edge_count));
               errors.emplace_back(error);
            }

            // Note: there used to be a check for non-duplicate points, but polygons sometimes have bow-tie forms.

            return errors;
         }

      }

      void face::make_faces(const map& m, faces& white, faces& black, faces& red, faces& exteriors)
      {
         // The edges we need to process.
         //
         // Because the edges are sorted by their p1, we can easily find all edges around the same point
         // as another edge by "looking around" the address of the edge in the vector.
         const std::vector<edge>& all_edges = m.all();
         if (all_edges.empty())
            return;

         const edge* const first_edge = &all_edges.front();
         const edge* const last_edge = &all_edges.back();

         auto edge_index = [first_edge=first_edge](const edge* edge) -> size_t
         {
            return edge - first_edge;
         };

         // Crossings where this is an odd number of edges cannot propagate the checker-board
         // pattern properly. We will ignore them during propagation to avoid completely random
         // checker-boards.
         auto is_p1_odd_crossing = [first_edge=first_edge,last_edge=last_edge] (const edge* e) -> bool
         {
            const point& p1 = e->p1;
            int count = 1;
            for (const edge* pe = e - 1; pe >= first_edge && pe->p1 == p1; --pe)
               count += 1;
            for (const edge* ne = e + 1; ne <= last_edge  && ne->p1 == p1; ++ne)
               count += 1;
            return (count & 1) == 1;
         };

         auto next_edge_around_p1 = [first_edge=first_edge,last_edge=last_edge] (const edge* e) -> const edge*
         {
            const point& p1 = e->p1;
            if (e < last_edge && (e+1)->p1 == p1)
               return (e + 1);
            while (e > first_edge)
               if ((e - 1)->p1 == p1)
                  --e;
               else
                  break;
            return e;
         };

         auto find_twin = [first_edge=first_edge,last_edge=last_edge] (const edge twin) -> const edge*
         {
            const edge* iter = std::lower_bound(first_edge, last_edge, twin);
            return iter;
         };

         // Keeping track of which edge needs to be done and which are already done.
         std::deque<const edge *> edges_todo;
         std::vector<bool> done_edges(all_edges.size(), false);
         std::vector<bool> seen_edges(all_edges.size(), false);
         std::vector<bool> edge_colors(all_edges.size(), false);

         auto add_point = [&edge_index, &done_edges, &seen_edges, &is_p1_odd_crossing](polygon& poly, const edge* cur_edge, size_t cur_edge_index) -> bool
         {
            poly.points.emplace_back(cur_edge->p1);

            done_edges[cur_edge_index] = true;
            seen_edges[cur_edge_index] = true;

            return is_p1_odd_crossing(cur_edge);
         };

         for (const edge* edge_iter = first_edge; edge_iter != last_edge; ++edge_iter)
         {
            const size_t edge_iter_index = edge_index(edge_iter);
            if (done_edges[edge_iter_index])
               continue;
            seen_edges[edge_iter_index] = true;

            edges_todo.emplace_back(edge_iter);

            while (edges_todo.size() > 0)
            {
               const edge* cur_edge = edges_todo.front();
               size_t cur_index = edge_index(cur_edge);
               const bool is_white = edge_colors[cur_index];
               edges_todo.pop_front();

               if (done_edges[cur_index])
                  continue;

               polygon new_face;
               const point until_point = cur_edge->p1;
               bool face_has_odd_crossing = add_point(new_face, cur_edge, cur_index);
               while (cur_edge->p2 != until_point)
               {
                  const edge* twin = find_twin(cur_edge->twin());
                  const size_t twin_index = edge_index(twin);
                  if (!done_edges[twin_index] && !seen_edges[twin_index])
                  {
                     edge_colors[edge_index(twin)] = !is_white;
                     seen_edges[twin_index] = true;
                     edges_todo.emplace_back(twin);
                  }

                  const edge* next_edge = next_edge_around_p1(twin);
                  if (new_face.points.size() > 50000)
                     break;
                  cur_edge = next_edge;
                  cur_index = edge_index(next_edge);
                  face_has_odd_crossing |= add_point(new_face, cur_edge, cur_index);
               }

               // Note: we don't want to have the exterior polygon!
               // TODO: better detection of the exterior polygon than using the number of points...
               if (new_face.points.size() > 50)
               {
                  exteriors.emplace_back(new_face);
                  continue;
               }

               // Odd-crossing faces are not part of the black/white division.
               if (face_has_odd_crossing)
               {
                  red.emplace_back(new_face);
                  continue;
               }

               if (is_white)
                  white.emplace_back(new_face);
               else
                  black.emplace_back(new_face);
            }
         }

         automatic_verify(m, white, black, red, exteriors);
      }

      std::vector<std::wstring> face::verify(const map& map, const faces& white, const faces& black, const faces& red, const faces& exteriors)
      {
         return internal_verify(map, white, black, red, exteriors);
      }

      void face::verify_and_throw(const map& map, const faces& white, const faces& black, const faces& red, const faces& exteriors)
      {
         const auto errors = verify(map, white, black, red, exteriors);
         if (errors.size() > 0)
            throw std::exception(L::t("Faces are invalid."));
      }

   }
}

// vim: sw=3 : sts=3 : et : sta : 
