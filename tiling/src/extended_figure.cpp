#include <dak/tiling/extended_figure.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/intersect.h>
#include <dak/geometry/transform.h>

#include <dak/utility/text.h>

#include <cmath>
#include <algorithm>
#include <sstream>

namespace dak
{
   namespace tiling
   {
      using geometry::transform_t;
      using geometry::point_t;
      using namespace geometry::intersect;
      using geometry::edge_t;
      using geometry::edges_map_t;
      using geometry::PI;
      using geometry::TOLERANCE;
      using utility::L;

      extended_figure_t::extended_figure_t(const std::shared_ptr<radial_figure_t>& c)
      : scale_figure_t(c ? c->n : 6), child(c)
      {
         child_changed();
      }

      extended_figure_t::extended_figure_t(const extended_figure_t& other)
      : scale_figure_t(other), child()
      {
         if (other.child)
            child = std::dynamic_pointer_cast<radial_figure_t>(other.child->clone());
      }

      extended_figure_t& extended_figure_t::operator=(const extended_figure_t& other)
      {
         scale_figure_t::operator=(other);
         if (other.child)
            child = std::dynamic_pointer_cast<radial_figure_t>(other.child->clone());
         else
            child = nullptr;
         return *this;
      }

      std::shared_ptr<figure_t> extended_figure_t::clone() const
      {
         return std::make_shared<extended_figure_t>(*this);
      }

      void extended_figure_t::child_changed()
      {
         cached_s_last_build_unit = NAN;
      }

      bool extended_figure_t::operator==(const figure_t& other) const
      {
         const auto other_extended = dynamic_cast<const extended_figure_t *>(&other);
         if (!other_extended)
            return false;

         if (!child || !other_extended->child)
            return false;

         return *child == *other_extended->child;
      }

      bool extended_figure_t::is_similar(const figure_t& other) const
      {
         const auto other_extended = dynamic_cast<const extended_figure_t *>(&other);
         if (!other_extended)
            return false;

         if (!child || !other_extended->child)
            return false;

         return child->is_similar(*other_extended->child);
      }

      void extended_figure_t::make_similar(const figure_t& other)
      {
         const auto other_extended = dynamic_cast<const extended_figure_t *>(&other);
         if (!other_extended)
            return;

         if (!child || !other_extended->child)
            return;

         child->make_similar(*other_extended->child);
      }

      std::wstring extended_figure_t::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Extended Rosette") << L" " << n;
         return ss.str();
      }

      double extended_figure_t::compute_scale() const
      {
         return compute_scale(child);
      }

      bool extended_figure_t::is_cache_valid() const
      {
         return scale_figure_t::is_cache_valid()
             && (!child || child->is_cache_valid())
             && cached_s_last_build_unit == compute_scale();
      }

      void extended_figure_t::update_cached_values() const
      {
         scale_figure_t::update_cached_values();
         cached_s_last_build_unit = compute_scale();
      }

      const radial_figure_t* extended_figure_t::get_child() const 
      { 
         return child.get();
      }

      edges_map_t extended_figure_t::build_unit() const
      {
         edges_map_t cunit = scale_figure_t::build_unit();

         // We want the tip of the new figure to still be at (1,0).
         const double s = compute_scale();
         if (!utility::near(s, 1.0))
            cunit.apply_to_self(transform_t::rotate(PI / n));

         scale_to_unit(cunit);

         return cunit;
      }

      double extended_figure_t::compute_scale(std::shared_ptr<radial_figure_t> child)
      {
         if (!child)
            return 1.;

         // Find the vertex at (1,0), extend its incoming edges, intersect
         // with rotations of same, and use the location of the intersection
         // to compute a scale factor.
         edges_map_t cunit = child->build_unit();

         const point_t tip_pos = point_t::unit_x();
         const transform_t trf = transform_t::rotate(2.0 * PI / child->n);

         // Find the tip, i.e. the vertex at (1,0)
         for (const edge_t& edge : cunit.outbounds(tip_pos))
         {
            const point_t& bpos = edge.p2;
            if (bpos.y < 0.0)
            {
               const point_t seg_end = tip_pos + (tip_pos - bpos).normalize().scale(100.0);
               const point_t neg_seg = point_t(seg_end.x, -seg_end.y);

               const point_t ra = tip_pos.apply(trf);
               const point_t rb = neg_seg.apply(trf);

               const point_t isect = intersect(tip_pos, seg_end, ra, rb);
               return isect.is_invalid() ? 1.0 : std::cos(PI / child->n) / isect.mag();
            }
         }

         return 1.0;
      }

      void extended_figure_t::scale_to_unit(edges_map_t& cunit)
      {
         const auto iter = std::max_element(cunit.all().begin(), cunit.all().end(), [](const auto& lhs, const auto& rhs) {
            return std::max(lhs.p1.x, lhs.p2.x) < std::max(rhs.p1.x, rhs.p2.x);
         });

         if (iter == cunit.all().end())
            return;

         cunit.apply_to_self(transform_t::scale(1.0 / std::max(iter->p1.x, iter->p2.x)));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

