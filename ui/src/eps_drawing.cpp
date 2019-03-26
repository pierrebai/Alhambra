#include <dak/ui/eps_drawing.h>

namespace dak
{
   namespace ui
   {
      eps_drawing::eps_drawing(std::wostream& out)
      : out(out)
      {
         // Postscript header and space to write bouncing box.
         out << L"%!PS-Adobe-3.0 EPSF-3.0\n";
         out << L"                                                               \n";
         out << L"                                                               \n";
         out << L"                                                               \n";

         // Our simplified, shortened commands.
         out << L"/bd { bind def } bind def\n"
                L"/co { setrgbcolor } bd\n"
                L"/lw { setlinewidth } bd\n"
                L"/st { stroke } bd\n"
                L"/fi { fill } bd\n"
                L"/cp { closepath } bd\n"
                L"/mo { newpath moveto } bd\n"
                L"/to { lineto } bd\n"
                L"/li { mo to st } bd\n"
                L"/li2 { mo to to st } bd\n"
                L"/ci { newpath 0 360 arc } bd\n"
                L"/ar { newpath arc } bd\n"
                L"/sm { setmatrix } bd\n";

         // EPS requires we save teh state before drawing.
         out << L"gsave\n";
      }

      eps_drawing::~eps_drawing()
      {
         // Restore drawing state.
         out << L"grestore\n";

         // Rewrite the Postscript header with the real bounding box.
         out.seekp(0);
         out << L"%!PS-Adobe-3.0 EPSF-3.0\n";
         out << L"%%BoundingBox: "
             << (int)std::floor(bbox.x - 12)             << L" " << (int)std::floor(bbox.y - 12)
             << (int)std::ceil(bbox.x + bbox.width + 12) << L" " << (int)std::ceil(bbox.y + bbox.height + 12)
             << L"\n";
      }

      drawing& eps_drawing::draw_line(const point& from, const point& to)
      {
         internal_update_stroke();
         internal_update_color();
         internal_update_transform();
         internal_update_bbox(from);
         internal_update_bbox(to);

         out << from.x << L" " << from.y << L" " << to.x << L" " << to.y << L" li\n";

         return *this;
      }

      drawing& eps_drawing::draw_corner(const point& from, const point& mid, const point& to)
      {
         internal_update_stroke();
         internal_update_color();
         internal_update_transform();
         internal_update_bbox(from);
         internal_update_bbox(mid);
         internal_update_bbox(to);

         out << from.x << L" " << from.y << L" " << mid.x << L" " << mid.y << L" " << to.x << L" " << to.y << L" li2\n";

         return *this;
      }

      drawing& eps_drawing::fill_polygon(const polygon& p)
      {
         const auto& pts = p.points;

         if (pts.size() <= 0)
            return *this;

         internal_update_stroke();
         internal_update_color();
         internal_update_transform();
         internal_update_bbox(pts);

         out << pts[0].x << L" " << pts[0].y << L" mo\n";
         for (int i = 1; i < pts.size(); ++i) {
            out << pts[i].x << L" " << pts[i].y << L" to\n";
         }
         out << L"cp fi\n";

         return *this;
      }

      drawing& eps_drawing::draw_polygon(const polygon& p)
      {
         const auto& pts = p.points;

         if (pts.size() <= 0)
            return *this;

         internal_update_stroke();
         internal_update_color();
         internal_update_transform();
         internal_update_bbox(pts);

         out << pts[0].x << L" " << pts[0].y << L" mo\n";
         for (int i = 1; i < pts.size(); ++i) {
            out << pts[i].x << L" " << pts[i].y << L" to\n";
         }
         out << L"cp st\n";

         return *this;
      }

      drawing& eps_drawing::fill_oval(const point& c, double rx, double ry)
      {
         internal_update_stroke();
         internal_update_color();
         internal_update_transform();
         internal_update_bbox(c);
         internal_update_bbox(c + point(rx, ry));
         internal_update_bbox(c - point(rx, ry));

         out << c.x << L" " << c.y << L" " << rx << L" ci fi\n";

         return *this;
      }

      drawing& eps_drawing::draw_oval(const point& c, double rx, double ry)
      {
         internal_update_stroke();
         internal_update_color();
         internal_update_transform();

         out << c.x << L" " << c.y << L" " << rx << L" ci st\n";

         return *this;
      }

      drawing& eps_drawing::fill_arc(const point& c, double rx, double ry, double angle1, double angle2)
      {
         out << c.x << L" " << c.y << L" " << rx << L" " << angle1 << L" " << angle2 << L" ar fi\n";

         return *this;
      }

      drawing& eps_drawing::fill_rect(const rect& r)
      {
         fill_polygon(polygon::from_rect(r));

         return *this;
      }

      drawing& eps_drawing::draw_rect(const rect& r)
      {
         draw_polygon(polygon::from_rect(r));

         return *this;
      }

      // Note: the bounds are *without* the transform.
      //       They are the true bounds of the drawing surface.
      rect eps_drawing::get_bounds() const
      {
         return bbox;
      }

      void eps_drawing::internal_update_bbox(const std::vector<point>& pts)
      {
         for (const auto& p : pts)
            internal_update_bbox(p);
      }

      void eps_drawing::internal_update_bbox(const point& p)
      {
         bbox = bbox.combine(point(p.x, p.y));
      }

      void eps_drawing::internal_update_color()
      {
         if (get_color() == applied_co)
            return;

         applied_co = get_color();

         out << (applied_co.r / 255.) << L" "
             << (applied_co.g / 255.) << L" "
             << (applied_co.b / 255.) << L" "
             << L" co\n";
      }

      void eps_drawing::internal_update_stroke()
      {
         if (get_stroke() == applied_strk)
            return;

         applied_strk = get_stroke();

         if (applied_strk.width == std::floor(applied_strk.width))
            out << (int)applied_strk.width << L" lw\n";
         else
            out << applied_strk.width << L" lw\n";

         switch (applied_strk.cap)
         {
            case stroke::cap_style::flat:
               out << L"0 setlinecap\n";
               break;
            case stroke::cap_style::round:
               out << L"1 setlinecap\n";
               break;
            case stroke::cap_style::square:
               out << L"2 setlinecap\n";
               break;
         }

         switch (applied_strk.join)
         {
            case stroke::join_style::miter:
               out << L"0 setlinejoin\n";
               break;
            case stroke::join_style::round:
               out << L"1 setlinejoin\n";
               break;
            case stroke::join_style::bevel:
               out << L"2 setlinejoin\n";
               break;
         }
      }

      void eps_drawing::internal_update_transform()
      {
         if (get_transform() == applied_trf)
            return;

         applied_trf = get_transform();
         out << L"[ " << applied_trf.scale_x << L" " << applied_trf.rot_1 << L" " << applied_trf.trans_x << L" "
                      << applied_trf.rot_2 << L" " << applied_trf.scale_y << L" " << applied_trf.trans_y << L" ] sm\n";
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
