#include <dak/geometry/face.h>
#include <dak/geometry/utility.h>

#include <algorithm>
#include <numeric>
#include <deque>
#include <map>

namespace dak
{
   namespace geometry
   {
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
            //wchar_t error[200];

            //// Note: exterior faces sometimes contain duplicate points...
            //face::faces clean_exteriors(exteriors);
            //for (auto& face : clean_exteriors)
            //{
            //   auto& sorted_points = face.points;
            //   std::sort(sorted_points.begin(), sorted_points.end());
            //   sorted_points.erase(std::unique(sorted_points.begin(), sorted_points.end()), sorted_points.end());
            //}

            //face::faces faces(white);
            //faces.insert(faces.end(), black.begin(), black.end());
            //faces.insert(faces.end(), red.begin(), red.end());
            //faces.insert(faces.end(), clean_exteriors.begin(), clean_exteriors.end());

            //if (white.size() > black.size() * 2)
            //{
            //   _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"White faces contains more than twice black faces (%ld vs %ld)."), (long) white.size(), (long) black.size());
            //   errors.emplace_back(error);
            //}
            //if (black.size() > white.size() * 2)
            //{
            //   _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Black faces contains more than twice white faces (%ld vs %ld)."), (long) black.size(), (long) white.size());
            //   errors.emplace_back(error);
            //}

            // Make sure there are no duplicate points in each face.
            //for (const auto& face : faces)
            //{
            //   auto sorted_points = face.points;

            //   std::sort(sorted_points.begin(), sorted_points.end());
            //   if (std::unique(sorted_points.begin(), sorted_points.end()) != sorted_points.end())
            //   {
            //      const point& pt = sorted_points.back();
            //      _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Face contains duplicate point %f/%f."), pt.x, pt.y);
            //      errors.emplace_back(error);
            //   }
            //}

            // Make sure there are exactly as many points in the faces as in the map.
            //{
            //   const size_t map_edge_count = map.canonicals().size() + map.non_canonicals().size();
            //   const size_t faces_edge_count = std::accumulate(faces.begin(), faces.end(), size_t(0), [](const size_t& a, const polygon&b) { return a + b.points.size(); });
            //   const size_t exteriors_edge_count = std::accumulate(exteriors.begin(), exteriors.end(), size_t(0), [](const size_t& a, const polygon&b) { return a + b.points.size(); });
            //   const size_t clean_exteriors_edge_count = std::accumulate(clean_exteriors.begin(), clean_exteriors.end(), size_t(0), [](const size_t& a, const polygon&b) { return a + b.points.size(); });
            //   const size_t adjusted_faces_edge_count = faces_edge_count + exteriors_edge_count - clean_exteriors_edge_count;
            //   //const size_t exteriors_edge_count = std::accumulate(exteriors.begin(), exteriors.end(), size_t(0), [](const size_t& a, const polygon&b) { return a + b.points.size(); });
            //   if (map_edge_count != faces_edge_count && map_edge_count != adjusted_faces_edge_count)
            //   {
            //      _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Face and map do not contain the same number of points: %ld vs. %ld."),
            //                   static_cast<long>(faces_edge_count), static_cast<long>(map_edge_count));
            //      errors.emplace_back(error);
            //   }
            //}
            
            // TODO: Make sure all points in all faces are consecutive.

            return errors;
         }

         typedef std::vector<std::pair<edge, edge>> next_edges;
         typedef std::vector<std::pair<point, int>> crossing_counts;

         next_edges build_next_edge_around_points(const std::vector<edge>& all_edges, crossing_counts& crossing_counts)
         {
            next_edges next_edge_around_points;

            edge first_edge;
            edge prev_edge;
            int crossing_count = 0;
            for (const auto& cur_edge : all_edges)
            {
               if (prev_edge.is_invalid())
               {
                  first_edge = cur_edge;
                  crossing_count = 1;
               }
               else
               {
                  if (prev_edge.p1 == cur_edge.p1)
                  {
                     next_edge_around_points.emplace_back(prev_edge, cur_edge);
                     crossing_count++;
                  }
                  else
                  {
                     crossing_counts.emplace_back(first_edge.p1, crossing_count);
                     next_edge_around_points.emplace_back(prev_edge, first_edge);
                     first_edge = cur_edge;
                     crossing_count = 1;
                  }
               }
               prev_edge = cur_edge;
            }

            if (!prev_edge.is_invalid())
            {
               crossing_counts.emplace_back(first_edge.p1, crossing_count);
               next_edge_around_points.emplace_back(prev_edge, first_edge);
            }

            std::sort(crossing_counts.begin(), crossing_counts.end());
            return next_edge_around_points;
         }
      }

      void face::make_faces(const map& m, faces& white, faces& black, faces& red, faces& exteriors)
      {
         // The points we need to process.
         std::vector<edge> all_edges;
         all_edges.reserve(m.canonicals().size() + m.non_canonicals().size());
         all_edges.insert(all_edges.end(), m.canonicals().begin(), m.canonicals().end());
         all_edges.insert(all_edges.end(), m.non_canonicals().begin(), m.non_canonicals().end());
         std::sort(all_edges.begin(), all_edges.end());

         auto edge_index = [&all_edges=all_edges](const edge& edge)
         {
            const auto iter = std::lower_bound(all_edges.begin(), all_edges.end(), edge);
            return std::distance(all_edges.begin(), iter);
         };

         // Sorted vector that can be used to find rapidly the next outbound edge that
         // follow a given outbound edge around a point. (To find from an inbound,
         // search using the twin of the edge.)
         //
         // Note: because the input is sorted by edge, the output is automatically
         //       sorted by points too. This allows us to use std::lower_bound to 
         //       find the next edge.
         crossing_counts crossing_counts;
         const auto next_edge_around_points = build_next_edge_around_points(all_edges, crossing_counts);

         // Crossings where this is an odd number of edges cannot propagate the checker-board
         // pattern properly. We will ignore them during propagation to avoid completely random
         // checker-boards.
         auto is_odd_crossing = [&crossing_counts=crossing_counts] (const point& pt) -> bool
         {
            const auto iter = std::lower_bound(crossing_counts.begin(), crossing_counts.end(), std::make_pair(pt, 0));
            const int crossing_count = (iter != crossing_counts.end()) ? iter->second : 0;
            return crossing_count > 1 && (crossing_count & 1) != 0;
         };

         // Keeping track of which edge needs to be done and which are already done.
         std::deque<edge> edges_todo;
         std::deque<bool> whites_todo;
         std::vector<bool> done_edges(all_edges.size(), false);
         std::vector<bool> seen_edges(all_edges.size(), false);
         std::vector<bool> edge_colors(all_edges.size(), false);

         const auto all_edges_end = all_edges.end();
         for (auto edge_iter = all_edges.begin(); edge_iter != all_edges_end; ++edge_iter)
         {
            {
               const size_t index = std::distance(all_edges.begin(), edge_iter);
               if (done_edges[index])
                  continue;
               seen_edges[index] = true;
            }

            edges_todo.emplace_back(*edge_iter);
            whites_todo.emplace_back(true);

            while (edges_todo.size() > 0)
            {
               const auto first_edge = edges_todo.front();
               const bool should_be_white = whites_todo.front();
               edges_todo.pop_front();
               whites_todo.pop_front();

               const size_t first_index = edge_index(first_edge);
               if (done_edges[first_index])
                  continue;

               auto add_point = [&edge_index, &done_edges, &seen_edges, &is_odd_crossing](polygon& poly, const edge& cur_edge) -> bool
               {
                  poly.points.emplace_back(cur_edge.p1);

                  const size_t index = edge_index(cur_edge);
                  done_edges[index] = true;
                  seen_edges[index] = true;

                  return is_odd_crossing(cur_edge.p1);
               };

               polygon new_face;
               bool face_has_odd_crossing = add_point(new_face, first_edge);

               bool is_white = should_be_white;
               for (edge cur_edge = first_edge; cur_edge.p2 != first_edge.p1; )
               {
                  const edge twin = cur_edge.twin();
                  if (done_edges[edge_index(twin)])
                     is_white = !edge_colors[edge_index(twin)];
                  const auto next_iter = std::lower_bound(next_edge_around_points.begin(), next_edge_around_points.end(), std::make_pair(twin, edge::lowest_edge(cur_edge.p2)));
                  const edge next_edge = next_iter->second;
                  if (new_face.points.size() > 50000)
                     break;
                  face_has_odd_crossing |= add_point(new_face, next_edge);
                  cur_edge = next_edge;
               }

               // Note: we don't want to have the exterior polygon!
               // TODO: better detection of the exterior polygon than using the number of points...
               if (new_face.points.size() > 50)
               {
                  exteriors.emplace_back(new_face);
                  continue;
               }

               if (face_has_odd_crossing)
               {
                  red.emplace_back(new_face);
                  continue;
               }

               if (is_white)
                  white.emplace_back(new_face);
               else
                  black.emplace_back(new_face);

               // Propagate to twins of the edge of the new face, but not if it has an odd crossing.
               point p1 = new_face.points.back();
               for (const point& p2 : new_face.points)
               {
                  edge_colors[edge_index(edge(p1, p2))] = is_white;
                  const edge twin(p2, p1);
                  const size_t twin_index = edge_index(twin);
                  if (!done_edges[twin_index] && !seen_edges[twin_index])
                  {
                     seen_edges[twin_index] = true;
                     edges_todo.emplace_back(twin);
                     whites_todo.emplace_back(!is_white);
                  }
                  p1 = p2;
               }
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
