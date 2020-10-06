#include <dak/tiling/irregular_figure.h>

#include <dak/tiling/rosette.h>
#include <dak/tiling/extended_figure.h>
#include <dak/tiling/star.h>
#include <dak/tiling/infer.h>

#include <dak/utility/text.h>

#include <sstream>

namespace dak
{
   namespace tiling
   {
      using utility::L;

      std::shared_ptr<figure_t> irregular_figure_t::clone() const
      {
         return std::make_shared<irregular_figure_t>(*this);
      }

      bool irregular_figure_t::operator==(const figure_t& other) const
      {
         const auto other_irregular = dynamic_cast<const irregular_figure_t *>(&other);
         if (!other_irregular)
            return false;

         return poly  == other_irregular->poly
             && infer == other_irregular->infer
             && q     == other_irregular->q
             && d     == other_irregular->d
             && s     == other_irregular->s;
      }

      bool irregular_figure_t::is_similar(const figure_t& other) const
      {
         const auto other_irregular = dynamic_cast<const irregular_figure_t *>(&other);
         if (!other_irregular)
            return false;

         return poly == other_irregular->poly;
      }

      void irregular_figure_t::make_similar(const figure_t& other)
      {
         if (const auto other_rosette = dynamic_cast<const rosette_t *>(&other))
         {
            q = other_rosette->q;
            s = other_rosette->s;
         }
         else if (const auto other_star = dynamic_cast<const star_t *>(&other))
         {
            d = other_star->d;
            s = other_star->s;
         }
         else if (const auto other_extended = dynamic_cast<const extended_figure_t *>(&other))
         {
            if (other_extended->child)
            {
               make_similar(*other_extended->child);
            }
         }
         else if (const auto other_irregular = dynamic_cast<const irregular_figure_t *>(&other))
         {
            q = other_irregular->q;
            d = other_irregular->d;
            s = other_irregular->s;
            infer = other_irregular->infer;
         }
      }

      std::wstring irregular_figure_t::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Irregular") << L" " << L::t(infer_mode_name(infer)) << L" " << poly.points.size();
         return ss.str();
      }

      bool irregular_figure_t::is_cache_valid() const
      {
         return cached_infer == infer
             && cached_q == q
             && cached_d == d
             && cached_s == s
             && cached_poly == poly
             && figure_t::is_cache_valid();
      }

      void irregular_figure_t::update_cached_values() const
      {
         cached_poly = poly;
         cached_infer = infer;
         cached_q = q;
         cached_d = d;
         cached_s = s;
      }

      void irregular_figure_t::build_map() const
      {
         if (!mosaic)
            return;

         dak::tiling::infer_t inf(mosaic, poly);

         switch (infer)
         {
            case infer_mode_t::star:
               cached_map = inf.inferStar(poly, d, s);
               break;
            case infer_mode_t::girih:
               cached_map = inf.inferGirih(poly, int(poly.points.size()), d);
               break;
            case infer_mode_t::intersect:
               cached_map = inf.inferIntersect(poly, int(poly.points.size()), d, s);
               break;
            case infer_mode_t::progressive:
               cached_map = inf.inferIntersectProgressive(poly, int(poly.points.size()), d, s);
               break;
            case infer_mode_t::hourglass:
               cached_map = inf.inferHourglass(poly, d, s);
               break;
            case infer_mode_t::rosette:
            case infer_mode_t::extended_rosette:
               cached_map = inf.inferRosette(poly, q, s, d);
               break;
            case infer_mode_t::simple:
               cached_map = inf.simple_infer(poly);
               break;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
