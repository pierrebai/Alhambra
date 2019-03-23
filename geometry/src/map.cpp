#include <dak/geometry/map.h>
#include <dak/geometry/intersect.h>
#include <dak/geometry/utility.h>

#include <algorithm>

namespace dak
{
   namespace geometry
   {
      const edge edge::invalid;

      //#define DAK_GEOMETRY_MAP_INTERNAL_VERIFY

      ////////////////////////////////////////////////////////////////////////////
      //
      // map edges.

      void map_edges::internal_clear_inverted_edges()
      {
         _sorted_inverted_edges.clear();
      }

      void map_edges::internal_fill_sorted_inverted_edges() const
      {
         if (_sorted_inverted_edges.size() > 0)
            return;

         _sorted_inverted_edges.reserve(_sorted_canonical_edges.size());
         for (const auto& edge : _sorted_canonical_edges)
            _sorted_inverted_edges.emplace_back(edge.twin());
         std::sort(_sorted_inverted_edges.begin(), _sorted_inverted_edges.end());
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // map.

      map::map(const edges& from_edges)
      {
         insert(from_edges);
      }

      bool map::are_connected(const point& a, const point& b) const
      {
         const edge e( (a < b) ? edge(a, b) : edge(b, a) );
         const auto iter = std::lower_bound(_sorted_canonical_edges.begin(), _sorted_canonical_edges.end(), e);
         return iter != _sorted_canonical_edges.end() && *iter == e;
      }

      bool map::contains(const point& p) const
      {
         return internal_contains(_sorted_canonical_edges, p)
             || internal_contains(non_canonicals(),  p);
      }

      bool map::internal_contains(const edges& from_edges, const point& p)
      {
         const auto iter = std::lower_bound(from_edges.begin(), from_edges.end(), edge::lowest_edge(p));
         return (iter != from_edges.end() && iter->p1 == p);
      }

      bool map::contains(const edge& e) const
      {
         if (e.is_canonical())
         {
            const auto iter = std::lower_bound(_sorted_canonical_edges.begin(), _sorted_canonical_edges.end(), e);
            return iter != _sorted_canonical_edges.end() && *iter == e;
         }
         else
         {
            const auto iter = std::lower_bound(non_canonicals().begin(), non_canonicals().end(), e);
            return iter != non_canonicals().end() && *iter == e;
         }
      }

      map::edges map::connections(const point& p) const
      {
         edges conns;
         internal_fill_connections(_sorted_canonical_edges, p, conns, true, true);
         internal_fill_connections(non_canonicals(), p, conns, true, true);
         return conns;
      }

      map::edges map::inbounds(const point& p) const
      {
         edges conns;
         internal_fill_connections(_sorted_canonical_edges, p, conns, true, false);
         internal_fill_connections(non_canonicals(), p, conns, true, false);
         return conns;
      }

      map::edges map::outbounds(const point& p) const
      {
         edges conns;
         internal_fill_connections(_sorted_canonical_edges, p, conns, false, true);
         internal_fill_connections(non_canonicals(), p, conns, false, true);
         return conns;
      }

      std::pair<edge, edge> map::before_after(const edge& e) const
      {
         edges conns = outbounds(e.p2);
         return before_after(conns, e);
      }

      std::pair<edge, edge> map::before_after(edges& outbounds, const edge& e)
      {
         std::sort(outbounds.begin(), outbounds.end());
         const auto iter = std::lower_bound(outbounds.begin(), outbounds.end(), e.twin());
         if (iter == outbounds.end())
            return std::pair<edge, edge>();

         const auto before = (iter == outbounds.begin()) ? outbounds.back() : *std::prev(iter);
         const auto after = (iter == std::prev(outbounds.end())) ? outbounds.front() : *std::next(iter);
         return std::pair<edge, edge>(before, after);
      }

      const edge& map::before(const edge& e) const
      {
         edges conns = outbounds(e.p2);
         return before(conns, e);
      }

      const edge& map::before(edges& outbounds, const edge& e)
      {
         std::sort(outbounds.begin(), outbounds.end());
         const auto iter = std::lower_bound(outbounds.begin(), outbounds.end(), e.twin());
         if (iter == outbounds.end())
            return edge::invalid;

         return (iter == outbounds.begin()) ? outbounds.back() : *std::prev(iter);
      }

      const edge& map::after(const edge& e) const
      {
         edges conns = outbounds(e.p2);
         return after(conns, e);
      }

      const edge& map::after(edges& outbounds, const edge& e)
      {
         std::sort(outbounds.begin(), outbounds.end());
         const auto iter = std::lower_bound(outbounds.begin(), outbounds.end(), e.twin());
         if (iter == outbounds.end())
            return edge::invalid;

         return (iter == std::prev(outbounds.end())) ? outbounds.front() : *std::next(iter);
      }

      const edge& map::continuation(const edge& e) const
      {
         edges conns = outbounds(e.p2);
         return continuation(conns, e);
      }

      const edge& map::continuation(edges& outbounds, const edge& e)
      {
         std::sort(outbounds.begin(), outbounds.end());
         const auto iter = std::lower_bound(outbounds.begin(), outbounds.end(), e.twin());
         if (iter == outbounds.end())
            return edge::invalid;

         const ptrdiff_t delta = outbounds.size() / 2;
         const auto opposite = ((iter - outbounds.begin()) >= delta) ? std::prev(iter, delta) : std::next(iter, delta);
         return *opposite;
      }

      void map::remove(const point& p)
      {
         // Note: we could improve the performance by directly erasing lower_bound to upper_bound,
         //       and removing the other edges after.
         const edges to_remove = connections(p);
         for (const auto& edge : to_remove)
            remove(edge);
      }

      void map::remove(const edge& e)
      {
         edge inverted(e.twin());
         internal_remove(_sorted_canonical_edges, e.is_canonical() ? e : inverted);
         internal_clear_inverted_edges();
         internal_verify();
      }

      void map::internal_remove(edges& from_edges, const edge& e)
      {
         auto i = std::lower_bound(from_edges.begin(), from_edges.end(), e);
         if (i != from_edges.end() && *i == e)
            from_edges.erase(i);
      }

      void map::merge(const map& other)
      {
         std::vector<std::pair<edge, point>> temp_edge_intersections;
         temp_edge_intersections.reserve(_sorted_canonical_edges.size() + other._sorted_canonical_edges.size());
         for (const auto& new_edge : other._sorted_canonical_edges)
            if (!contains(new_edge))
               internal_connect(new_edge, temp_edge_intersections);
         internal_add_intersections_and_sort(temp_edge_intersections);
         internal_clear_inverted_edges();
         internal_verify();
      }

      void map::insert(const edges& from_edges)
      {
         std::vector<std::pair<edge, point>> temp_edge_intersections;
         temp_edge_intersections.reserve(_sorted_canonical_edges.size() + from_edges.size());
         for (const auto& new_edge : from_edges)
         {
            if (new_edge.is_trivial())
               return;

            if (contains(new_edge))
               continue;

            temp_edge_intersections.clear();
            internal_connect(new_edge.canonical(), temp_edge_intersections);
            internal_add_intersections_and_sort(temp_edge_intersections);
         }
         internal_clear_inverted_edges();
         internal_verify();
      }

      void map::insert(const edge& new_edge)
      {
         if (new_edge.is_trivial())
            return;

         if (contains(new_edge))
            return;

         std::vector<std::pair<edge, point>> temp_edge_intersections;
         internal_connect(new_edge.canonical(), temp_edge_intersections);
         internal_add_intersections_and_sort(temp_edge_intersections);
         internal_clear_inverted_edges();
         internal_verify();
      }

      void map::connect(const point& p1, const point& p2)
      {
         insert(edge(p1, p2));
      }

      map& map::apply_to_self(const transform& t)
      {
         for (auto& e : _sorted_canonical_edges)
            e = edge(e.p1.apply(t), e.p2.apply(t)).make_canonical();
         internal_sort_edges();
         internal_clear_inverted_edges();
         internal_verify();
         return *this;
      }

      map map::apply(const transform& t) const
      {
         map other(*this);
         other.apply_to_self(t);
         return other;
      }

      void map::internal_sort_edges()
      {
         std::sort(_sorted_canonical_edges.begin(), _sorted_canonical_edges.end());
         _sorted_canonical_edges.erase(std::unique(_sorted_canonical_edges.begin(), _sorted_canonical_edges.end()), _sorted_canonical_edges.end());
      }

      void map::internal_fill_connections(const edges& from_edges, const point& p, edges& conns, bool inbound, bool outbound)
      {
         const auto lower = std::lower_bound(from_edges.begin(), from_edges.end(), edge::lowest_edge(p));
         const auto upper = std::upper_bound(from_edges.begin(), from_edges.end(), edge::highest_edge(p));
         for (auto i = lower; i < upper; ++i)
         {
            if (i->p1 == p)
            {
               if (outbound)
                  conns.emplace_back(*i);
               if (inbound)
                  conns.emplace_back(i->twin());
            }
         }
      }

      // Split existing edges and record the intersections for each edge, old and new.
      void map::internal_connect(const edge& new_edge, std::vector<std::pair<edge, point>>& temp_edge_intersections)
      {
         const point& new_p1 = new_edge.p1;
         const point& new_p2 = new_edge.p2;

         const auto new_min_max_x = std::minmax(new_p1.x, new_p2.x);
         const auto new_min_max_y = std::minmax(new_p1.y, new_p2.y);

         int intersection_count = 0;

         for (auto& my_edge : _sorted_canonical_edges)
         {
            const point& my_p1 = my_edge.p1;
            const point& my_p2 = my_edge.p2;

            if (std::min(my_p1.x, my_p2.x) > new_min_max_x.second)
               break;

            if (std::max(my_p1.x, my_p2.x) < new_min_max_x.first)
               continue;

            if (std::min(my_p1.y, my_p2.y) > new_min_max_y.second)
               continue;

            if (std::max(my_p1.y, my_p2.y) < new_min_max_y.first)
               continue;

            const point ipt = intersect::intersect_within(new_p1, new_p2, my_p1, my_p2);

            if (ipt.is_invalid())
               continue;

            if (ipt != my_p1 && ipt != my_p2)
               temp_edge_intersections.emplace_back(my_edge, ipt);
            if (ipt != new_p1 && ipt != new_p2)
               temp_edge_intersections.emplace_back(new_edge, ipt);
            intersection_count++;
         }

         if (intersection_count == 0)
         {
            // If no intersection found, just add the new edge with an invalid intersection point.
            temp_edge_intersections.emplace_back(new_edge, point());
         }
      }

      void map::internal_add_intersections_and_sort(std::vector<std::pair<edge, point>>& temp_edge_intersections)
      {
         // Sort intersections.
         std::sort(temp_edge_intersections.begin(), temp_edge_intersections.end());

         // We will build a new vector of sorted edges.
         // It will be filled by iterating over the existing sorted edges and the intersections
         // in parallel so that edges are inserted in already sorted order.
         edges new_sorted_canonical_edges;
         new_sorted_canonical_edges.reserve(_sorted_canonical_edges.size() + temp_edge_intersections.size());

         // Keep track of previous edge and point so we can detect when
         // we've finished splitting one particular edge.
         edge prev_edge;
         point prev_point;
         point last_point;

         auto inter_iter = temp_edge_intersections.begin();
         const auto inter_end = temp_edge_intersections.end();

         auto edges_iter = _sorted_canonical_edges.begin();
         const auto edges_end = _sorted_canonical_edges.end();

         while (true)
         {
            const bool valid_edges = (edges_iter != edges_end);
            const bool valid_inter = (inter_iter != inter_end);
            const bool do_edges = valid_edges && (!valid_inter || *edges_iter < inter_iter->first);
            if (do_edges)
            {
               const edge& old_edge = *edges_iter;
               new_sorted_canonical_edges.emplace_back(old_edge);
               ++edges_iter;
            }
            else if (valid_inter)
            {
               // Skip over sorted edge that is split, if any.
               if (valid_edges && *edges_iter == inter_iter->first)
               {
                  ++edges_iter;
               }

               const edge& split_edge = inter_iter->first;
               const point& intersection = inter_iter->second;

               if (intersection.is_invalid())
               {
                  // If the edge was not split, then it was a new edge
                  // that didn't touch any existing edges.
                  // Just add it straight to the map.
                  new_sorted_canonical_edges.emplace_back(split_edge.canonical());
               }
               else
               {
                  // If the edge was split, we need to remove it and replace
                  // it by new smaller edges.

                  // First, check if we've changed which edge we're splitting.
                  if (split_edge != prev_edge)
                  {
                     // Finish any previous splitted edge.
                     if (!prev_point.is_invalid())
                     {
                        new_sorted_canonical_edges.emplace_back(edge(prev_point, last_point).canonical());
                     }
                     prev_edge = split_edge;
                     prev_point = split_edge.p1 < split_edge.p2 ? split_edge.p1 : split_edge.p2;
                     last_point = split_edge.p1 < split_edge.p2 ? split_edge.p2 : split_edge.p1;
                  }

                  // Split the input edge at every intersection. Beware that an edge might get split twice
                  // if it is intersected by an end-point of the other map, which result in two identical
                  // intersections.
                  if (prev_point != intersection)
                     new_sorted_canonical_edges.emplace_back(edge(prev_point, intersection).canonical());
                  prev_point = intersection;
               }
               ++inter_iter;
            }
            else
            {
               break;
            }
         }

         // Finish any previous splitted edge.
         if (!prev_point.is_invalid())
         {
            new_sorted_canonical_edges.emplace_back(edge(prev_point, last_point).canonical());
         }

         _sorted_canonical_edges.swap(new_sorted_canonical_edges);
         internal_sort_edges();
      }

      void map::internal_verify() const
      {
         #ifdef DAK_GEOMETRY_MAP_INTERNAL_VERIFY
            verify_and_throw();
         #endif
      }

      void map::verify_and_throw() const
      {
         if (verify().size() > 0)
            throw std::exception(L::t("Map is invalid."));
      }

      std::vector<std::wstring> map::verify() const
      {
         std::vector<std::wstring> errors;

         wchar_t error[200];

         // Make sure there are no trivial edges.
         for (const auto& e : _sorted_canonical_edges)
         {
            if (e.p1 == e.p2)
            {
               _snwprintf_s(error, sizeof(error)/sizeof(error[0]), L::t(L"Trivial edge %f/%f - %f/%f."), e.p1.x, e.p1.y, e.p2.x, e.p2.y);
               errors.emplace_back(error);
            }

            if (e.p1.is_invalid() || e.p2.is_invalid())
            {
               _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Invalid edge %f/%f - %f/%f."), e.p1.x, e.p1.y, e.p2.x, e.p2.y);
               errors.emplace_back(error);
            }
         }

         // Make sure there are no trivial edges.
         for (const auto& e : non_canonicals())
         {
            if (e.p1 == e.p2)
            {
               _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Trivial edge %f/%f - %f/%f."), e.p1.x, e.p1.y, e.p2.x, e.p2.y);
               errors.emplace_back(error);
            }

            if (e.p1.is_invalid() || e.p2.is_invalid())
            {
               _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Invalid edge %f/%f - %f/%f."), e.p1.x, e.p1.y, e.p2.x, e.p2.y);
               errors.emplace_back(error);
            }
         }

         // Make sure the edges are in sorted order.
         {
            edge prev;
            for (const auto& e : _sorted_canonical_edges)
            {
               if (!prev.is_invalid())
               {
                  if (e < prev)
                  {
                     _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Canonical edges are not sorted."));
                     errors.emplace_back(error);
                  }
               }
               prev = e;
            }
         }

         // Make sure the edges are in sorted order.
         {
            edge prev;
            for (const auto& e : non_canonicals())
            {
               if (!prev.is_invalid())
               {
                  if (e < prev)
                  {
                     _snwprintf_s(error, sizeof(error) / sizeof(error[0]), L::t(L"Inverted edges are not sorted."));
                     errors.emplace_back(error);
                  }
               }
               prev = e;
            }
         }

         return errors;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
