#pragma once

#ifndef DAK_GEOMETRY_MAP_H
#define DAK_GEOMETRY_MAP_H

#include <dak/geometry/edge.h>

#include <vector>
#include <string>

namespace dak
{
   namespace geometry
   {
      class point;
      class transform;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Base class that make sure non-canonicals edge are calculated on-demand.

      class map_edges
      {
      public:
         // Container of edges.
         typedef std::vector<edge> edges;

         // Retrieve the vector of canonical edges. (p1 < p2)
         const edges& canonicals() const { return _sorted_canonical_edges; }

         // Retrieve the vector of non-canonical edges. (p1 > p2)
         // Calculated on-demand.
         const edges& non_canonicals() const { internal_fill_sorted_inverted_edges(); return _sorted_inverted_edges; }

      protected:
         // Clear the non-canonical edges to be recalculated, after the canonical edges have changed.
         void internal_clear_inverted_edges();

         edges _sorted_canonical_edges;

      private:
         // Calculate the non-canonical edges.
         void internal_fill_sorted_inverted_edges() const;

         mutable edges _sorted_inverted_edges;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // The implementation of a planar map abstraction.  A planar map is
      // an (undirected) graph represented on the plane in such a way that
      // edges don't cross points or other edges.
      //
      // What we actually stored are directed edges sorted by the x position
      // of the first point of the edge, but each connection between two
      // points are kept twice, once for each direction.

      class map : public map_edges
      {
      public:
         // Empty map.
         map() { }

         // Map with all given edges inserted.
         map(const edges& from_edges);

         // Verify if two points are connected by an edge.
         bool are_connected(const point& a, const point& b) const;

         // Verify if a point is in the map. (End-point of an edge)
         bool contains(const point& a) const;

         // Verify if an edge is already in the map.
         bool contains(const edge& e) const;

         // Returns all edges connected to the point.
         edges connections(const point& p) const;

         // Returns all edges that have the point as their second point.
         edges inbounds(const point& p) const;

         // Returns all edges that have the point as their first point.
         edges outbounds(const point& p) const;

         // Return the preceeding a succeeding edge of an edge.
         std::pair<edge, edge> before_after(const edge& e) const;
         static std::pair<edge, edge> before_after(edges& outbounds, const edge& e);

         const edge& before(const edge& e) const;
         static const edge& before(edges& outbounds, const edge& e);

         const edge& after(const edge& e) const;
         static const edge& after(edges& outbounds, const edge& e);

         // Return the edge that is opposite of an edge at the p2 intersection.
         const edge& continuation(const edge& e) const;
         static const edge& continuation(edges& outbounds, const edge& e);

         // Remove a point and all connected edges.
         void remove(const point& p);

         // Remove an edge.
         void remove(const edge& e);

         // Merge two maps, adding necessary intersection points as needed.
         void merge(const map& other);

         // Merge two maps, assuming no too edges are the same nor intersect.
         void merge_non_overlapping(const map& other);

         // Insert a group of edges, adding necessary intersection points as needed.
         void insert(const edges& from_edges);

         // Insert one edge, adding necessary intersection points as needed.
         void insert(const edge& new_edge);

         // Insert one edge between two points, adding necessary intersection points as needed.
         void connect(const point& p1, const point& p2);

         // Apply a transform to the map, creating a new map.
         map apply(const transform& t) const;

         // Apply a transform to the map itself.
         map& apply_to_self(const transform& t);

         // Verify if there are any errors in the map.
         // Return the list of errors.
         std::vector<std::wstring> verify() const;

         // Verify if there are any errors in the map.
         // Throw an exception if there is at least on error.
         void verify_and_throw() const;

      private:
         // Sort the canonical edges in ascending order.
         void internal_sort_edges();

         // Fill the list of connections of the requested kind.
         static void internal_fill_connections(const edges& from_edges, const point& p, edges& conns, bool inbound, bool outbound);

         // Internal remove implementation.
         static void internal_remove(edges& from_edges, const edge& e);

         // Internal containment check implementation.
         static bool internal_contains(const edges& from_edges, const point& p);

         // Split existing edges and record the intersections for each edge,
         // old and new.
         void internal_connect(const edge& new_edge, std::vector<std::pair<edge, point>>& temp_edge_intersections);

         // Insert new edges and intersections.
         void internal_add_intersections_and_sort(std::vector<std::pair<edge, point>>& temp_edge_intersections);

         // Internal implementation of the validity verifications.
         void internal_verify() const;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
