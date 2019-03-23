#include <dak/tiling/irregular_figure.h>

#include <dak/tiling/rosette.h>
#include <dak/tiling/extended_figure.h>
#include <dak/tiling/star.h>
#include <dak/tiling/infer.h>

#include <dak/geometry/utility.h>

#include <sstream>

namespace dak
{
   namespace tiling
   {
      using geometry::L;

      std::shared_ptr<figure> irregular_figure::clone() const
      {
         return std::make_shared<irregular_figure>(*this);
      }

      bool irregular_figure::is_similar(const figure& other) const
      {
         const auto other_irregular = dynamic_cast<const irregular_figure *>(&other);
         if (!other_irregular)
            return false;

         return poly == other_irregular->poly;
      }

      void irregular_figure::make_similar(const figure& other)
      {
         if (const auto other_rosette = dynamic_cast<const rosette *>(&other))
         {
            q = other_rosette->q;
            s = other_rosette->s;
         }
         else if (const auto other_star = dynamic_cast<const star *>(&other))
         {
            d = other_star->d;
            s = other_star->s;
         }
         else if (const auto other_extended = dynamic_cast<const extended_figure *>(&other))
         {
            if (other_extended->child)
            {
               make_similar(*other_extended->child);
            }
         }
         else if (const auto other_irregular = dynamic_cast<const irregular_figure *>(&other))
         {
            q = other_irregular->q;
            d = other_irregular->d;
            s = other_irregular->s;
            infer = other_irregular->infer;
         }
      }

      std::wstring irregular_figure::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Irregular") << L" " << L::t(infer_mode_name(infer)) << L" " << poly.points.size();
         return ss.str();
      }

      bool irregular_figure::is_cache_valid() const
      {
         return cached_infer == infer
             && cached_q == q
             && cached_d == d
             && cached_s == s
             && cached_poly == poly
             && figure::is_cache_valid();
      }

      void irregular_figure::update_cached_values() const
      {
         cached_poly = poly;
         cached_infer = infer;
         cached_q = q;
         cached_d = d;
         cached_s = s;
      }

      void irregular_figure::build_map() const
      {
         if (!mosaic)
            return;

         dak::tiling::infer inf(mosaic, poly);

         switch (infer)
         {
            case infer_mode::star:
               cached_map = inf.inferStar(poly, d, s);
               break;
            case infer_mode::girih:
               cached_map = inf.inferGirih(poly, int(poly.points.size()), d);
               break;
            case infer_mode::intersect:
               cached_map = inf.inferIntersect(poly, int(poly.points.size()), d, s);
               break;
            case infer_mode::progressive:
               cached_map = inf.inferIntersectProgressive(poly, int(poly.points.size()), d, s);
               break;
            case infer_mode::hourglass:
               cached_map = inf.inferHourglass(poly, d, s);
               break;
            case infer_mode::rosette:
            case infer_mode::extended_rosette:
               cached_map = inf.inferRosette(poly, q, s, d);
               break;
            case infer_mode::simple:
               cached_map = inf.simple_infer(poly);
               break;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
