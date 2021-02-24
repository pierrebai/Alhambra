#include <dak/tiling_ui_qt/tiling_editor.h>

#include <dak/ui/qt/drawing_canvas.h>
#include <dak/ui/qt/convert.h>

#include <dak/tiling/tiling_selection.h>

#include <dak/geometry/utility.h>

#include <dak/utility/containers.h>
#include <dak/utility/text.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qactiongroup.h>

#include <sstream>

namespace dak
{
   namespace tiling_ui_qt
   {
      using namespace dak::ui;
      using namespace dak::ui::qt;
      using namespace dak::tiling;
      using namespace dak::QtAdditions;

      using dak::geometry::polygon_t;
      using dak::geometry::transform_t;
      using dak::geometry::point_t;
      using dak::geometry::edge_t;

      using dak::utility::L;

      typedef dak::ui::mouse::buttons_t mouse_buttons_t;
      typedef dak::ui::mouse::event_t mouse_event_t;

      namespace
      {
         ////////////////////////////////////////////////////////////////////////////
         //
         // Mouse mode, triggered by the toolbar or user interaction.

         enum class mouse_mode_t
         {
            normal            = 0,
            copy_polygon      = 1,
            move_polygon      = 2,
            delete_polygon    = 3,
            draw_fill_vectors = 4,
            draw_polygon      = 5,
            include_polygon   = 6,
            pan_view          = 7,
            rotate_view       = 8,
            zoom_view         = 9,
         };

         ////////////////////////////////////////////////////////////////////////////
         //
         // Mouse interactions. (See below.)

         class mouse_action_t;
         class mouse_receiver_t;

         static constexpr double selection_distance = 8.;

      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // UI implementation of the tiling editor.

      class tiling_editor_ui_t : public drawing_canvas_t
      {
      public:
         tiling_editor_ui_t(QWidget* parent);

         // Tiling management.
         void set_tiling(const tiling_t& tiling);
         tiling_t create_tiling();
         bool verify_tiling(const std::wstring& operation);
         void show_entire_tiling();

         // Error reporting.
         void report_error(const std::wstring& message, message_reporter_t::category_t cat = message_reporter_t::category_t::error);
         void paint(QPainter& painter) override;
         void draw(drawing_t& drw) override;
         static void draw_placed_tile(drawing_t& drw, const placed_tile_t& tile, const color_t& col);

         // Create copies of the feature to help visualise the result in the panel.
         void fill_using_translations();

         void remove_excluded();
         void exclude_all();
         bool is_included(const std::shared_ptr<placed_tile_t>& placed) const;

         // Tiling translation vector.
         void add_to_translation(const point_t& wpt, bool ending);

         // const polygon_t& management.
         std::shared_ptr<placed_tile_t> add_placed_tile(const placed_tile_t&);
         void remove_placed_tile(const std::shared_ptr<placed_tile_t>& placed);
         void remove_placed_tile(const selection_t& sel);

         void toggle_inclusion(const std::shared_ptr<placed_tile_t>&);

         void create_polygon_copies();

         // Validating that features don't overlap when tiled.
         void update_overlap(std::shared_ptr<placed_tile_t>& placed);
         void update_overlaps();
         bool is_overlapping(const std::shared_ptr<placed_tile_t>& placed) const;

         // Tiling translation vector.
         bool is_translation_invalid();

         // Mouse mode handling.
         void update_mouse_mode(QAction* action, mouse_mode_t mode);
         void update_selection_under_mouse(const selection_t& sel);
         void update_selection_from_point(const point_t& wpt);

         // Internal translation.
         point_t get_translation_1() const;
         point_t get_translation_2() const;

         // Possible user actions.
         void toggle_inclusion(const selection_t& sel);
         void clear_translation();
         void update_polygon_sides(int number);
         void add_regular_polygon();
         void delete_polygon(const selection_t& sel);
         void copy_polygon(const selection_t& sel);
         void forget_polygon();

         // Mouse events.
         void start_mouse_interaction(const point_t& wpt, mouse_buttons_t mouseButton);
         selection_t find_selection_under_mouse(selection_type_t sel_types);
         selection_t find_selection(const point_t& wpt, selection_type_t sel_types);
         selection_t find_selection(const point_t& wpt, const selection_t& excluded, selection_type_t sel_types);

         std::shared_ptr<mouse_action_t>& get_mouse_interaction() { return mouse_interaction; }
         void clear_mouse_interaction() { mouse_interaction = nullptr; }

         // Convert back a model position in screen coordinates.
         point_t world_to_screen(const point_t& wpt);
         point_t screen_to_world(const point_t wpt);

         // Various colors used to draw the tiling.
         static constexpr color_t in_tiling_color = color_t::from_fractions(1.0f, 0.85f, 0.85f);
         static constexpr color_t overlapping_color = color_t::from_fractions(1.0f, 0.4f, 0.1f, 0.9f);
         static constexpr color_t under_mouse_color = color_t::from_fractions(0.5f, 1.0f, 0.5f, 0.6f);
         static constexpr color_t drag_color = color_t::from_fractions(0.8f, 0.7f, 0.4f, 0.9f);
         static constexpr color_t construction_color = color_t::green().percent(80);
         static constexpr color_t normal_color = color_t::from_fractions(0.85f, 0.85f, 1.0f, 0.9f);

         // The edited tiling.
         tiling_t edited;
         std::vector<std::shared_ptr<placed_tile_t>> tiles;
         std::set<std::shared_ptr<placed_tile_t>> overlaps;
         std::set<std::shared_ptr<placed_tile_t>> inclusions;

         // Current mouse selection_t.
         selection_t   current_selection;
         selection_t   under_mouse;

         // Interaction mode.
         mouse_mode_t current_mouse_mode = mouse_mode_t::normal;

         // Accumulation of number of sides to create a regular polygon.
         int poly_side_count = 0;

         // Accumulation of selected vertices when drawing_t polygons.
         polygon_t new_polygon;

         // Translation vector so that the tiling tiles the plane.
         bool   drawing_translation = false;
         point_t  trans1_start;
         point_t  trans1_end;
         point_t  trans2_start;
         point_t  trans2_end;

      private:
         // Mouse interaction underway.
         std::shared_ptr<mouse_action_t>   mouse_interaction;
         std::shared_ptr<mouse_receiver_t> receiver;

         // Error reporting.
         message_reporter_t reporter;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Mouse interactions.
      //
      // Called by the mouse_receiver_t class. Each sub-class of the mouse_action_t
      // embodies a particular interaction: drawing a polygon, moving a polygon,
      // etc.

      namespace
      {
         class mouse_action_t
         {
         public:
            mouse_action_t(tiling_editor_ui_t& editor, const selection_t& sel, const point_t& wpt)
            : editor(editor), cur_sel(sel), last_drag(wpt)
            {
               editor.update();
            }

            virtual void update_dragging(const point_t& wpt)
            {
               last_drag = wpt;
               editor.update();
            }

            virtual void end_dragging(const point_t& wpt)
            {
               editor.update();
               editor.clear_mouse_interaction();
            }

            virtual void draw(drawing_t&)
            {
               // Sub-classes add drawing_t.
            }

         protected:
            tiling_editor_ui_t& editor;
            selection_t         cur_sel;
            point_t             last_drag;
         };

         class move_polygon : public mouse_action_t
         {
         public:
            move_polygon(tiling_editor_ui_t& editor, const selection_t& sel, const point_t& wpt)
            : mouse_action_t(editor, sel, wpt)
            {
            }

            void update_dragging(const point_t& wpt) override
            {
               if (const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(cur_sel))
               {
                  point_t diff = wpt - last_drag;
                  if (!diff.is_invalid())
                     placed->trf = transform_t::translate(diff).compose(placed->trf);
               }
               mouse_action_t::update_dragging(wpt);
            }
         };

         class copy_move_polygon : public move_polygon
         {
         public:
            transform_t initial_transform;

            transform_t current_trf()
            {
               const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(cur_sel);
               if (!placed)
                  return transform_t::identity();

               return placed->trf;
            }

            copy_move_polygon(tiling_editor_ui_t& editor, const selection_t& sel, const point_t& wpt)
            : move_polygon(editor, sel, wpt), initial_transform(current_trf())
            {
               editor.copy_polygon(sel);
            }

            void end_dragging(const point_t& wpt) override
            {
               point_t initial_pos = editor.world_to_screen(point_t::origin().apply(initial_transform));
               point_t final_pos = editor.world_to_screen(point_t::origin().apply(current_trf()));
               if (initial_pos.distance_2(final_pos) > 49.)
               {
                  // Nothing to do to confirm placed tile.
               }
               else
               {
                  editor.remove_placed_tile(cur_sel);
                  cur_sel = selection_t();
               }
               move_polygon::end_dragging(wpt);
            }
         };

         class draw_translation : public mouse_action_t
         {
         public:
            draw_translation(tiling_editor_ui_t& editor, const selection_t& sel, const point_t& wpt)
            : mouse_action_t(editor, sel, wpt)
            {
               const point_t pt = tiling_selection::get_point(sel);
               if (!pt.is_invalid())
                  editor.add_to_translation(pt, false);
            }

            void update_dragging(const point_t& wpt) override
            {
               selection_t sel = editor.find_selection(wpt, selection_type_t::point);
               const point_t pt = tiling_selection::get_point(sel);
               if (!pt.is_invalid())
                  editor.trans1_end = pt;

               mouse_action_t::update_dragging(wpt);
            }

            void draw(drawing_t& drw) override
            {
               if (editor.trans1_start.is_invalid() || last_drag.is_invalid())
                  return;

               const double arrow_length = drw.get_transform().dist_from_inverted_zero(12.0);
               const double arrow_width = drw.get_transform().dist_from_inverted_zero(6.0);
               drw.set_color(editor.drag_color);
               drw.draw_line(editor.trans1_start, last_drag);
               drw.fill_arrow(editor.trans1_start, last_drag, arrow_length, arrow_width);
            }

            void end_dragging(const point_t& wpt) override
            {
               selection_t sel = editor.find_selection(wpt, selection_type_t::point);
               const point_t pt = tiling_selection::get_point(sel);
               if (!pt.is_invalid())
                  editor.add_to_translation(pt, true);

               mouse_action_t::end_dragging(wpt);
            }
         };

         class join_edge : public mouse_action_t
         {
         public:
            join_edge(tiling_editor_ui_t& editor, const selection_t& sel, const point_t& wpt)
            : mouse_action_t(editor, sel, wpt)
            {
            }

            bool snap_to_edge(const point_t& wpt)
            {
               const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(cur_sel);
               if (!placed)
                  return false;

               if (placed->trf.is_invalid())
                  return false;

               selection_t new_sel = editor.find_selection(wpt, cur_sel, selection_type_t::edge);
               const std::shared_ptr<placed_tile_t>& new_placed = tiling_selection::get_placed_tile(new_sel);
               if (!new_placed)
                  return false;

               const edge_t p = tiling_selection::get_edge(cur_sel).raw_edge();
               if (p.is_invalid())
                  return false;

               const edge_t q = tiling_selection::get_edge(new_sel);
               if (q.is_invalid())
                  return false;

               transform_t match = transform_t::match_lines(p.p1, p.p2, q.p2, q.p1);
               if (match.is_invalid())
                  return false;

               placed->trf = match;

               return true;
            }

            void update_dragging(const point_t& wpt) override
            {
               if (cur_sel.has_selection())
               {
                  if (!snap_to_edge(wpt))
                  {
                     point_t diff = wpt - last_drag;
                     if (const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(cur_sel))
                        placed->trf = transform_t::translate(diff).compose(placed->trf);
                  }
               }
               mouse_action_t::update_dragging(wpt);
            }

            void end_dragging(const point_t& wpt) override
            {
               snap_to_edge(wpt);
               mouse_action_t::end_dragging(wpt);
            }
         };

         class copy_join_edge : public join_edge
         {
         public:
            const transform_t initial_transform;

            transform_t current_trf()
            {
               const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(cur_sel);
               if (!placed)
                  return transform_t::identity();

               return placed->trf;
            }

            copy_join_edge(tiling_editor_ui_t& editor, const selection_t& sel, const point_t& wpt)
            : join_edge(editor, sel, wpt), initial_transform(current_trf())
            {
               editor.copy_polygon(sel);
            }

            void end_dragging(const point_t& wpt) override
            {
               point_t initial_pos = editor.world_to_screen(point_t::origin().apply(initial_transform));
               point_t final_pos = editor.world_to_screen(point_t::origin().apply(current_trf()));
               if (initial_pos.distance_2(final_pos) < 49.)
               {
                  editor.remove_placed_tile(cur_sel);
                  cur_sel = selection_t();
               }
               join_edge::end_dragging(wpt);
            }
         };

         class draw_polygon : public mouse_action_t
         {
         public:

            draw_polygon(tiling_editor_ui_t& editor, const selection_t& sel, const point_t& wpt)
            : mouse_action_t(editor, sel, wpt)
            {
               add_vertex(sel);
            }

            void add_vertex(const selection_t& sel)
            {
               auto& pts = editor.new_polygon.points;
               const point_t pt = tiling_selection::get_point(sel);
               if (pt.is_invalid())
               {
                  pts.clear();
                  editor.clear_mouse_interaction();
               }
               else if (pts.size() > 2 && pt.distance_2(pts[0]) < geometry::TOLERANCE_2)
               {
                  editor.add_placed_tile(placed_tile_t{ editor.new_polygon, transform_t::identity() });
                  pts.clear();
                  cur_sel = selection_t();
                  editor.clear_mouse_interaction();
               }
               else if (!utility::contains(pts, pt))
               {
                  pts.emplace_back(pt);
                  editor.update();
               }
               cur_sel = sel;
            }

            void update_dragging(const point_t& wpt) override
            {
               selection_t sel = editor.find_selection(wpt, selection_type_t::point);
               const point_t pt = tiling_selection::get_point(sel);
               if (pt.is_invalid())
                  mouse_action_t::update_dragging(wpt);
               else
                  mouse_action_t::update_dragging(pt);
            }

            void draw(drawing_t& drw) override
            {
               if (editor.new_polygon.points.size() > 0)
               {
                  if (!last_drag.is_invalid())
                  {
                     double radius = drw.get_transform().dist_from_inverted_zero(selection_distance);
                     drw.set_color(editor.drag_color);
                     drw.set_stroke(3.);
                     drw.draw_line(editor.new_polygon.points.back(), last_drag);
                     drw.fill_oval(last_drag, radius, radius);
                  }
               }
            }

            void end_dragging(const point_t& wpt) override
            {
               add_vertex(editor.find_selection(wpt, selection_type_t::point));
               // Note: do *not* call mouse_action_t::end_dragging() to allow chaining
               //       drawing_t polygon_t sides.
            }
         };

         class mouse_receiver_t : public ui::mouse::receiver_t
         {
         public:
            tiling_editor_ui_t& editor;

            mouse_receiver_t(tiling_editor_ui_t& editor)
            : editor(editor)
            {
            }

            void mouse_pressed(const mouse_event_t& e) override
            {
               // Mouse events with shift key are used to pan/zoom/rotate the view
               // in transformer class registered as a mouse receiver in canvas.
               if (e.has_modifiers(modifiers_t::shift))
                  return;

               const point_t wpt = editor.screen_to_world(e.position);
               selection_t sel;

               // Note: the interaction can clear editor.mouse_interaction,
               //       so keep a copy of the shared pointer during the call.
               auto& inter = editor.get_mouse_interaction();

               switch (editor.current_mouse_mode)
               {
                  case mouse_mode_t::normal:
                     editor.start_mouse_interaction(wpt, e.buttons);
                     break;
                  case mouse_mode_t::copy_polygon:
                     if ((sel = editor.find_selection(wpt, selection_type_t::edge)).has_selection())
                        inter.reset(new copy_join_edge(editor, sel, wpt));
                     else if ((sel = editor.find_selection(wpt, selection_type_t::tile)).has_selection())
                        inter.reset(new copy_move_polygon(editor, sel, wpt));
                     break;
                  case mouse_mode_t::move_polygon:
                     if ((sel = editor.find_selection(wpt, selection_type_t::edge)).has_selection())
                        inter.reset(new join_edge(editor, sel, wpt));
                     else if ((sel = editor.find_selection(wpt, selection_type_t::tile)).has_selection())
                        inter.reset(new move_polygon(editor, sel, wpt));
                     break;
                  case mouse_mode_t::delete_polygon:
                     editor.delete_polygon(editor.find_selection(wpt, selection_type_t::tile));
                     break;
                  case mouse_mode_t::draw_fill_vectors:
                     if ((sel = editor.find_selection(wpt, selection_type_t::point)).has_selection())
                        inter.reset(new draw_translation(editor, sel, wpt));
                     break;
                  case mouse_mode_t::draw_polygon:
                     if ((sel = editor.find_selection(wpt, selection_type_t::point)).has_selection())
                        inter.reset(new draw_polygon(editor, sel, wpt));
                     break;
                  case mouse_mode_t::include_polygon:
                     editor.toggle_inclusion(editor.find_selection(wpt, selection_type_t::tile));
                     break;
                  case mouse_mode_t::pan_view:
                     break;
                  case mouse_mode_t::rotate_view:
                     break;
                  case mouse_mode_t::zoom_view:
                     break;
               }
            }

            void mouse_released(const mouse_event_t& e) override
            {
               const point_t wpt = editor.screen_to_world(e.position);
               // Note: the interaction can clear editor.mouse_interaction,
               //       so keep a copy of the shared pointer during the call.
               if (auto inter = editor.get_mouse_interaction())
               {
                  inter->end_dragging(wpt);
               }
            }

            void mouse_clicked(const mouse_event_t&) override
            {
               // nothing.
            }

            void mouse_dragged(const mouse_event_t& e) override
            {
               const point_t wpt = editor.screen_to_world(e.position);

               // Note: the interaction can clear editor.mouse_interaction,
               //       so keep a copy of the shared pointer during the call.
               auto inter = editor.get_mouse_interaction();
               if (!inter)
                  return editor.update_selection_from_point(wpt);

               switch (editor.current_mouse_mode)
               {
                  case mouse_mode_t::normal:
                  case mouse_mode_t::copy_polygon:
                  case mouse_mode_t::move_polygon:
                  case mouse_mode_t::draw_fill_vectors:
                  case mouse_mode_t::draw_polygon:
                     inter->update_dragging(wpt);
                     break;
                  default:
                     editor.update_selection_from_point(wpt);
                     break;
               }
            }

            void mouse_moved(const mouse_event_t& e) override
            {
               const point_t wpt = editor.screen_to_world(e.position);
               editor.update_selection_from_point(wpt);
            }

            void mouse_entered(const mouse_event_t& e) override
            {
               const point_t wpt = editor.screen_to_world(e.position);
               editor.update_selection_from_point(wpt);
            }

            void mouse_exited(const mouse_event_t& e) override
            {
               const point_t wpt = editor.screen_to_world(e.position);
               editor.update_selection_from_point(wpt);
            }

            void mouse_wheel(const mouse_event_t& e) override
            {
               const point_t wpt = editor.screen_to_world(e.position);
               editor.update_selection_from_point(wpt);
            }
         };
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Tiling editor UI.

      // Creation.
      tiling_editor_ui_t::tiling_editor_ui_t(QWidget* parent)
      : drawing_canvas_t(parent), reporter(this)
      {

         receiver = std::shared_ptr<mouse_receiver_t>(new mouse_receiver_t(*this));
         emi.event_receivers.push_back(receiver.get());
         set_tiling(tiling_t());
      }

      // Tiling management.

      void tiling_editor_ui_t::show_entire_tiling()
      {
         if (tiles.size() <= 0)
            return;

         // Find bounding-box of all tiles.
         geometry::rectangle_t bbox(0, 0, 0, 0);
         for (const auto& placed : tiles)
            bbox = placed->tile.apply(placed->trf).bounds().combine(bbox);

         // Make it so we can see 9 instances (3x3) of the tiling.
         bbox = bbox.central_scale(3.);
         geometry::rectangle_t region = convert(geometry());
         const transform_t fill_trf = transform_t::match_lines(bbox.top_left(), bbox.top_right(), region.top_left(), region.top_right());
         painter_trf_drawing.set_transform(fill_trf);
      }

      void tiling_editor_ui_t::set_tiling(const tiling_t& tiling)
      {
         edited = tiling;
         tiles.clear();
         current_selection = selection_t();
         under_mouse = selection_t();
         drawing_translation = false;

         point_t trans_origin;
         if (edited.tiles.size() > 0)
         {
            const auto& tile = tiling.tiles.begin()->first;
            const transform_t& trf = tiling.tiles.begin()->second.front();
            trans_origin = tile.center().apply(trf);
         }

         for (const auto& placed : edited.tiles)
         {
            for (const auto& trf : placed.second)
            {
               auto plt = std::make_shared<placed_tile_t>(placed_tile_t{placed.first, trf});
               tiles.push_back(plt);
               inclusions.insert(plt);
            }
         }

         trans1_start = trans_origin;
         trans1_end = trans_origin + tiling.t1;
         trans2_start = trans_origin;
         trans2_end = trans_origin + tiling.t2;

         create_polygon_copies();
         update_overlaps();
         show_entire_tiling();
         update();
      }

      tiling_t tiling_editor_ui_t::create_tiling()
      {
         // TODO: name, author, description.
         tiling_t new_tiling;
         new_tiling.t1 = get_translation_1();
         new_tiling.t2 = get_translation_2();
         for (const auto& placed : tiles)
            if (is_included(placed))
               new_tiling.tiles[placed->tile].emplace_back(placed->trf);
         return new_tiling;
      }

      bool tiling_editor_ui_t::verify_tiling(const std::wstring& operation)
      {
         if (tiles.size() <= 0)
         {
            if (operation.length())
               report_error(std::wstring(L::t(L"Cannot ")) + operation + L::t(L" the tiling without polygons!\n") +
                            L::t(L"Please add some polygons to the tiling.\n") +
                            L::t(L"(Enter a number and press <Enter> to create a regular polygon_t.)"));
            return false;
         }

         const size_t included_count = inclusions.size();
         if (included_count <= 0)
         {
            if (operation.length())
               report_error(std::wstring(L::t(L"Cannot ")) + operation + L::t(L" the tiling without selected tiles!\n") +
                            L::t(L"Please select some tiles for inclusion.\n") +
                            L::t(L"(Press the T key while the mouse is over a polygon_t.)"));
            return false;
         }

         if (is_translation_invalid())
         {
            if (operation.length())
               report_error(std::wstring(L::t(L"Cannot ")) + operation + L::t(L" the tiling without translation vectors!\n") +
                            L::t(L"Please define the two translation vectors.\n") +
                            L::t(L"(Middle mouse button on polygon_t vertex or center and drag to another.)"));
            return false;
         }

         return true;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Repaint.

      void tiling_editor_ui_t::paint(QPainter& painter)
      {
         drawing_canvas_t::paint(painter);
         reporter.paint(painter);
      }

      void tiling_editor_ui_t::draw_placed_tile(drawing_t& drw, const placed_tile_t& tile, const color_t& col)
      {
         const auto poly = tile.tile.apply(tile.trf);

         drw.set_color(col);
         drw.fill_polygon(poly);

         drw.set_stroke(2.);
         drw.set_color(color_t::black());
         drw.draw_polygon(poly);

         drw.set_color(color_t::red());
         const double radius = drw.get_transform().dist_from_inverted_zero( 6. );
         drw.draw_oval(poly.center(), radius, radius);
      }

      void tiling_editor_ui_t::draw(drawing_t& drw)
      {
         for (const auto& tile : tiles)
         {
            const color_t& co = is_overlapping(tile) ? overlapping_color
                            : is_included(tile)    ? in_tiling_color
                            : normal_color;
            draw_placed_tile(drw, *tile, co);
         }

         if (under_mouse.has_selection())
         {
            const point_t pt = tiling_selection::get_point(under_mouse);
            edge_t edge = tiling_selection::get_edge(under_mouse);
            const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(under_mouse);
            if (!pt.is_invalid())
            {
               double radius = drw.get_transform().dist_from_inverted_zero(selection_distance);
               drw.set_color(under_mouse_color);
               drw.fill_oval(pt, radius, radius);
            }
            else if (!edge.is_invalid())
            {
               drw.set_color(under_mouse_color);
               drw.set_stroke(selection_distance);
               drw.draw_line(edge.p1, edge.p2);
            }
            else if (placed)
            {
               draw_placed_tile(drw, *placed, under_mouse_color);
            }
         }

         double arrow_length = drw.get_transform().dist_from_inverted_zero(8.);
         double arrow_width = drw.get_transform().dist_from_inverted_zero(4.);
         if (!trans1_start.is_invalid() && !trans1_end.is_invalid())
         {
            drw.set_color(construction_color);
            drw.set_stroke(2.);
            drw.draw_line(trans1_start, trans1_end);
            drw.fill_arrow(trans1_start, trans1_end, arrow_length, arrow_width);
         }

         if (!trans2_start.is_invalid() && !trans2_end.is_invalid())
         {
            drw.set_color(construction_color);
            drw.set_stroke(2.);
            drw.draw_line(trans2_start, trans2_end);
            drw.fill_arrow(trans2_start, trans2_end, arrow_length, arrow_width);
         }

         if (mouse_interaction)
            mouse_interaction->draw(drw);
         
         if (new_polygon.points.size() > 0)
         {
            drw.set_color(construction_color);
            double radius = drw.get_transform().dist_from_inverted_zero(3.0);
            point_t prev = new_polygon.points.back();
            for (const auto& pt : new_polygon.points)
            {
               drw.fill_oval(pt, radius, radius);
               drw.set_stroke(3.);
               drw.draw_line(prev, pt);
               prev = pt;
            }
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Error reporting.

      void tiling_editor_ui_t::report_error(const std::wstring& text, message_reporter_t::category_t cat)
      {
         reporter.report(text, cat);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // const polygon_t& management.

      std::shared_ptr<placed_tile_t> tiling_editor_ui_t::add_placed_tile(const placed_tile_t& placed)
      {
         auto new_tile = std::make_shared<placed_tile_t>(placed);
         tiles.push_back(new_tile);

         update_overlaps();
         update();

         return new_tile;
      }

      void tiling_editor_ui_t::remove_placed_tile(const std::shared_ptr<placed_tile_t>& placed)
      {
         const std::shared_ptr<placed_tile_t>& placed_under_mouse = tiling_selection::get_placed_tile(under_mouse);
         if (placed && placed == placed_under_mouse)
            under_mouse = selection_t();

         // Note: do *not* use remove_if since there might be more than one polygon_t that matches.
         //       This happens with copy_move_polygon if the user doesn't move the mouse.
         const auto pos = std::find(tiles.begin(), tiles.end(), placed);
         if (pos != tiles.end())
            tiles.erase(pos);

         update_overlaps();
         update();
      }

      void tiling_editor_ui_t::remove_placed_tile(const selection_t& sel)
      {
         const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(sel);
         remove_placed_tile(placed);
      }

      void tiling_editor_ui_t::toggle_inclusion(const std::shared_ptr<placed_tile_t>& placed)
      {
         if (!placed)
            return;

         if (inclusions.count(placed))
            inclusions.erase(placed);
         else
            inclusions.insert(placed);

         update_overlaps();
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Create copies of the feature to help visualise the result in the panel.

      void tiling_editor_ui_t::fill_using_translations()
      {
         if (!verify_tiling(L::t(L"fill the surrounding of")))
            return;

         remove_excluded();
         create_polygon_copies();
      }

      void tiling_editor_ui_t::remove_excluded()
      {
         tiles.erase(std::remove_if(tiles.begin(), tiles.end(), [inclusions=inclusions](std::shared_ptr<placed_tile_t>& plt) { return inclusions.count(plt) <= 0; }), tiles.end());
         under_mouse = selection_t();
         current_selection = selection_t();

         update_overlaps();
         update();
      }

      void tiling_editor_ui_t::exclude_all()
      {
         inclusions.clear();

         update_overlaps();
         update();
      }

      bool tiling_editor_ui_t::is_included(const std::shared_ptr<placed_tile_t>& placed) const
      {
         return inclusions.count(placed) > 0;
      }


      void tiling_editor_ui_t::create_polygon_copies()
      {
         if (is_translation_invalid())
            return;

         const point_t t1 = get_translation_1();
         const point_t t2 = get_translation_2();

         // Note: make a copy because we are going to add new tiles which can reallocate the vector.
         std::vector<std::shared_ptr<placed_tile_t>> copy_tiles = tiles;
         for (const auto& placed : copy_tiles)
         {
            if (!is_included(placed))
               continue;

            const polygon_t& tile = placed->tile;
            const transform_t& trf = placed->trf;

            for (int y = -1; y <= 1; ++y)
            {
               for (int x = -1; x <= 1; ++x)
               {
                  if (y == 0 && x == 0)
                     continue;

                  const transform_t& placement = transform_t::translate(t1.scale(x) + t2.scale(y)).compose(trf);
                  tiles.push_back(std::make_shared<placed_tile_t>(placed_tile_t{tile, placement}));
               }
            }
         }

         update_overlaps();
         update();
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Validating that features don't overlap when tiled.
      //
      // Note: we're not very stringent to avoid flagging polygons that would
      //       only slightly overlap. This is more about including completely
      //       unnecessary polygons that cover area already covered by another
      //       copy, or other gross errors that are hard to see (especially once
      //       the plane if fully tiled!).

      void tiling_editor_ui_t::update_overlap(std::shared_ptr<placed_tile_t>& checked_tile)
      {
         if (!is_included(checked_tile))
            return;

         const point_t checked_center = checked_tile->tile.apply(checked_tile->trf).center();
         const bool trans_valid = !is_translation_invalid();
         const point_t t1 = trans_valid ? get_translation_1() : point_t::origin();
         const point_t t2 = trans_valid ? get_translation_2() : point_t::origin();

         for (auto& placed : tiles)
         {
            if (!is_included(placed))
               continue;

            if (*placed == *checked_tile)
               continue;

            const polygon_t tile = placed->tile.apply(placed->trf);

            for (int y = -1; y <= 1; ++y)
            {
               for (int x = -1; x <= 1; ++x)
               {
                  const transform_t trf = transform_t::translate(t1.scale(x) + t2.scale(y));
                  const polygon_t trans_tile = tile.apply(trf);
                  if (trans_tile.is_inside(checked_center))
                  {
                     overlaps.insert(checked_tile);
                     return;
                  }
               }
            }
         }
      }

      void tiling_editor_ui_t::update_overlaps()
      {
         overlaps.clear();

         for (auto& placed : tiles)
            update_overlap(placed);
      }

      bool tiling_editor_ui_t::is_overlapping(const std::shared_ptr<placed_tile_t>& placed) const
      {
         return overlaps.count(placed) > 0;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // const tiling& translation vector.
      //
      // We treat the two vector as a circular buffer so that each one will
      // get over-written in turn, so that both can be alternatively changed
      // by the end-user.

      void tiling_editor_ui_t::add_to_translation(const point_t& pt, bool ending)
      {
         if (trans1_start.is_invalid())
         {
            trans1_start = pt;
         }
         else if (trans1_end.is_invalid())
         {
            if (ending && pt != trans1_start)
            {
               trans1_end = pt;
            }
         }
         else if (!ending)
         {
            trans2_start = trans1_start;
            trans2_end = trans1_end;
            trans1_start = pt;
            trans1_end = point_t();
         }

         update_overlaps();
         update();
      }

      bool tiling_editor_ui_t::is_translation_invalid()
      {
         return trans1_start.is_invalid() || trans1_end.is_invalid()
             || trans2_start.is_invalid() || trans2_end.is_invalid();
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Mouse mode handling.

      void tiling_editor_ui_t::update_mouse_mode(QAction* action, mouse_mode_t mode)
      {
         bool selected = action->isChecked();
         mouse_mode_t new_mode = selected ? mode : mouse_mode_t::normal;
         if (new_mode == current_mouse_mode && new_mode != mouse_mode_t::normal)
         {
            new_mode = mouse_mode_t::normal;
            action->setChecked(false);
         }
         current_mouse_mode = new_mode;

         switch (current_mouse_mode)
         {
            case mouse_mode_t::pan_view:
               transformer.forced_interaction_mode = dak::ui::transformer_t::interaction_mode_t::moving;
               break;
            case mouse_mode_t::rotate_view:
               transformer.forced_interaction_mode = dak::ui::transformer_t::interaction_mode_t::rotating;
               break;
            case mouse_mode_t::zoom_view:
               transformer.forced_interaction_mode = dak::ui::transformer_t::interaction_mode_t::scaling;
               break;
            default:
               transformer.forced_interaction_mode = dak::ui::transformer_t::interaction_mode_t::normal;
               break;
         }
      }

      void tiling_editor_ui_t::update_selection_under_mouse(const selection_t& sel)
      {
         if (!sel.has_selection())
         {
            if (under_mouse.has_selection())
            {
               under_mouse = selection_t();
               update();
            }
         }
         else
         {
            under_mouse = sel;
            update();
         }
      }

      void tiling_editor_ui_t::update_selection_from_point(const point_t& wpt)
      {
         selection_t sel;

         switch (current_mouse_mode)
         {
            case mouse_mode_t::normal:
               update_selection_under_mouse(find_selection(wpt, selection_type_t::all));
               break;
            case mouse_mode_t::move_polygon:
            case mouse_mode_t::copy_polygon:
               if ((sel = find_selection(wpt, selection_type_t::edge_or_tile)).has_selection())
                  update_selection_under_mouse(sel);
               else
                  update_selection_under_mouse(find_selection(wpt, selection_type_t::edge_or_tile));
               break;
            case mouse_mode_t::delete_polygon:
            case mouse_mode_t::include_polygon:
               update_selection_under_mouse(find_selection(wpt, selection_type_t::tile));
               break;
            case mouse_mode_t::draw_fill_vectors:
               if ((sel = find_selection(wpt, selection_type_t::point)).has_selection())
                  update_selection_under_mouse(sel);
               else
                  update_selection_under_mouse(find_selection(wpt, selection_type_t::point));
               break;
            case mouse_mode_t::draw_polygon:
               if ((sel = find_selection(wpt, selection_type_t::point)).has_selection())
                  update_selection_under_mouse(sel);
               break;
            case mouse_mode_t::pan_view:
            case mouse_mode_t::rotate_view:
            case mouse_mode_t::zoom_view:
               sel = selection_t();
               update_selection_under_mouse(sel);
               break;
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Internal tiling creation.

      point_t tiling_editor_ui_t::get_translation_1() const
      {
         return trans1_end - trans1_start;
      }

      point_t tiling_editor_ui_t::get_translation_2() const
      {
         return trans2_end - trans2_start;
      }

      void tiling_editor_ui_t::toggle_inclusion(const selection_t& sel)
      {
         auto placed = tiling_selection::get_placed_tile(sel);
         if (!placed)
            return;

         toggle_inclusion(placed);
         update();
      }

      void tiling_editor_ui_t::clear_translation()
      {
         trans1_start = trans1_end = point_t();
         trans2_start = trans2_end = point_t();
         update_overlaps();
         update();
      }

      void tiling_editor_ui_t::update_polygon_sides(int number)
      {
         poly_side_count = poly_side_count * 10 + number;
         if (poly_side_count > 100)
            poly_side_count = number;
         std::wstring name;
         switch (poly_side_count) {
            case 0:
            case 1:
            case 2:     name = L::t(L"Add polygon_t");   break;
            case 3:     name = L::t(L"Add Triangle");  break;
            case 4:     name = L::t(L"Add Square");    break;
            case 5:     name = L::t(L"Add Pentagon");  break;
            case 6:     name = L::t(L"Add Hexagon");   break;
            case 7:     name = L::t(L"Add Heptagon");  break;
            case 8:     name = L::t(L"Add Octogon");   break;
            case 9:     name = L::t(L"Add Nonagon");   break;
            case 10:    name = L::t(L"Add Decagon");   break;
            case 12:    name = L::t(L"Add Dodecagon"); break;
            default:
            {
               std::wstringstream ss;
               ss << L::t(L"Add ") << poly_side_count << L::t("-gon");
               name = ss.str();
               break;
            }
         }
         if (tiling_editor_t* editor = dynamic_cast<tiling_editor_t *>(parent()))
            editor->add_poly_action->setText(QString::fromStdWString(name));
      }

      void tiling_editor_ui_t::add_regular_polygon()
      {
         if (poly_side_count > 2)
         {
            const polygon_t f = polygon_t::make_regular(poly_side_count);
            add_placed_tile({ f, transform_t::identity() });

            if (tiles.size() == 1)
               show_entire_tiling();
         }
         else
         {
            report_error(std::wstring(L::t(L"Cannot create a polygon_t with too few sides!\n")) +
                         std::wstring(L::t(L"Please enter a number of side on the keyboard before adding the polygon_t.\n")) +
                         std::wstring(L::t(L"(Enter a number and then press <Enter>.)")));
         }
         poly_side_count = 0;
         update_polygon_sides(0);
      }

      void tiling_editor_ui_t::delete_polygon(const selection_t& sel)
      {
         remove_placed_tile(sel);
      }

      void tiling_editor_ui_t::copy_polygon(const selection_t& sel)
      {
         const std::shared_ptr<placed_tile_t>& placed = tiling_selection::get_placed_tile(sel);
         if (!placed)
            return;
         auto new_tile = add_placed_tile(*placed);
         if (inclusions.count(placed))
            inclusions.insert(new_tile);
      }

      void tiling_editor_ui_t::forget_polygon()
      {
         new_polygon.points.clear();
         poly_side_count = 0;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Mouse tracking.

      selection_t tiling_editor_ui_t::find_selection_under_mouse(selection_type_t sel_types)
      {
         return find_selection(screen_to_world(transformer.get_tracked_point()), sel_types);
      }

      selection_t tiling_editor_ui_t::find_selection(const point_t& wpt, selection_type_t sel_types)
      {
         return find_selection(wpt, selection_t(), sel_types);
      }

      selection_t tiling_editor_ui_t::find_selection(const point_t& wpt, const selection_t& excluded, selection_type_t sel_types)
      {
         const double sel_dist = painter_trf_drawing.get_transform().dist_from_inverted_zero(selection_distance);
         return tiling_selection::find_selection(tiles, wpt, sel_dist, excluded, sel_types);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Mouse events.

      void tiling_editor_ui_t::start_mouse_interaction(const point_t& wpt, mouse_buttons_t mouseButton)
      {
         const selection_t& sel = find_selection(wpt, selection_type_t::all);
         if (sel.has_selection())
         {
            switch (mouseButton)
            {
               case mouse_buttons_t::one:
               {
                  // Drawing polygons is special because it requires multiple independent clicks.
                  // Otherwise, try to get what is under the mouse from more specific (point_t) to
                  // less specific (edge, then tile).
                  if (std::dynamic_pointer_cast<draw_polygon>(mouse_interaction))
                  {
                     mouse_interaction->update_dragging(wpt);
                     return;
                  }
                  else if (tiling_selection::get_point(sel))
                  {
                     mouse_interaction.reset(new draw_polygon(*this, sel, wpt));
                     return;
                  }
                  else if (tiling_selection::get_edge(sel))
                  {
                     mouse_interaction.reset(new join_edge(*this, sel, wpt));
                     return;
                  }
                  else if (tiling_selection::get_placed_tile(sel))
                  {
                     mouse_interaction.reset(new move_polygon(*this, sel, wpt));
                     return;
                  }
                  break;
               }

               case mouse_buttons_t::two:
               {
                  mouse_interaction.reset(new draw_translation(*this, sel, wpt));
                  return;
               }

               case mouse_buttons_t::three:
               {
                  if (tiling_selection::get_edge(sel))
                  {
                     mouse_interaction.reset(new copy_join_edge(*this, sel, wpt));
                     return;
                  }
                  else if (tiling_selection::get_placed_tile(sel))
                  {
                     mouse_interaction.reset(new copy_move_polygon(*this, sel, wpt));
                     return;
                  }
               }
            }
         }

         forget_polygon();
         mouse_interaction = nullptr;
      }

      point_t tiling_editor_ui_t::world_to_screen(const point_t& wpt)
      {
         return wpt.apply(painter_trf_drawing.get_transform());
      }

      point_t tiling_editor_ui_t::screen_to_world(const point_t spt)
      {
         return spt.apply(painter_trf_drawing.get_transform().invert());
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Tiling editor.

      // Creation.
      tiling_editor_t::tiling_editor_t(const tiling_editor_icons_t& icons, QWidget* parent)
      : QWidget(parent), ui(std::shared_ptr<tiling_editor_ui_t>(new tiling_editor_ui_t(this)))
      {
         build_actions(icons);
         build_ui();
      }

      void tiling_editor_t::set_tiling(const tiling_t& tiling)
      {
         ui->set_tiling(tiling);
      }
      
      tiling_t tiling_editor_t::create_tiling()
      {
         return ui->create_tiling();
      }

      bool tiling_editor_t::verify_tiling(const std::wstring& operation)
      {
         return ui->verify_tiling(operation);
      }

      void tiling_editor_t::report_error(const std::wstring& text, message_reporter_t::category_t cat)
      {
         ui->report_error(text, cat);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // End-user actions.

      void tiling_editor_t::build_ui()
      {
         auto layout = new QVBoxLayout(this);
         layout->addWidget(ui.get());
         layout->setMargin(0);
         setContentsMargins(0, 0, 0, 0);
      }

      void tiling_editor_t::build_actions(const tiling_editor_icons_t& icons)
      {
         clear_trans_action = CreateAction(L::t(L"Clear Vectors"), icons.clear_translation, 'U', L::t(L"Clear the translation vectors used to tile the plane. (Shortcut: U.)"), [ui=ui]()
         {
            ui->clear_translation();
         });

         fill_trans_action = CreateAction(L::t(L"Fill Tiling"), icons.fill_with_translation, 'F', L::t(L"Surround the design with copies using the translation vectors. (Shortcut: F.)"), [ui=ui]()
         {
            ui->fill_using_translations();
         });

         remove_excluded_action = CreateAction(L::t(L"Clean Tiling"), icons.remove_excluded, 'R', L::t(L"Remove all polygons that are not included in the tiling. (Shortcut: R)"), [ui=ui]()
         {
            ui->remove_excluded();
         });

         exclude_all_action = CreateAction(L::t(L"Exclude All"), icons.exclude_all, 'E', L::t(L"Exclude all polygons from the tiling. (Shortcut: E)"), [ui=ui]()
         {
            ui->exclude_all();
         });

         add_poly_action = CreateAction(L::t(L"Add polygon_t"), icons.add_polygon, QKeySequence(Qt::Key_Return), L::t(L"Add a regular polygon_t with X side, where X is a number you entered with the keyboard. (Shortcut: <Return>)"), [ui=ui]()
         {
            ui->add_regular_polygon();
         });

         draw_trans_toggle = CreateToggle(L::t(L"Draw Vectors"), icons.draw_translation, {}, L::t(L"Select the two translation vectors used to tile the plane, using the mouse. (Drag with the mouse.)"), [self=this, ui=ui]()
         {
            ui->update_mouse_mode(self->draw_trans_toggle, mouse_mode_t::draw_fill_vectors);
         });

         draw_poly_toggle = CreateToggle(L::t(L"Draw Polygon"), icons.draw_polygon, {}, L::t(L"Select a series of vertices counter-clockwise to draw a free-form polygon_t. (Click on vertices.)"), [self=this, ui=ui]()
         {
            ui->update_mouse_mode(self->draw_poly_toggle, mouse_mode_t::draw_polygon);
         });

         copy_poly_action = CreateAction(L::t(L"Copy Polygon"), icons.copy_polygon, 'C', L::t(L"Copy a polygon_t by drag-and-drop with the mouse. (Press C or drag with the mouse.)"), [self = this, ui = ui]()
         {
            selection_t sel = ui->find_selection_under_mouse(selection_type_t::tile);
            ui->copy_polygon(sel);
         });

         copy_poly_toggle = CreateToggle(L::t(L"Copy Polygon"), icons.copy_polygon, {}, L::t(L"Copy a polygon_t by drag-and-drop with the mouse. (Press C or drag with the mouse.)"), [self = this, ui = ui]()
         {
            selection_t sel = ui->find_selection_under_mouse(selection_type_t::tile);
            ui->update_mouse_mode(self->copy_poly_toggle, mouse_mode_t::copy_polygon);
         });

         move_poly_toggle = CreateToggle(L::t(L"Move Polygon"), icons.move_polygon, {}, L::t(L"Move a polygon_t by drag-and-drop with the mouse. (Drag with the mouse.)"), [self=this, ui=ui]()
         {
            ui->update_mouse_mode(self->move_poly_toggle, mouse_mode_t::move_polygon);
         });

         delete_poly_action = CreateAction(L::t(L"Delete Polygon"), icons.delete_polygon, 'D', L::t(L"Delete polygons by clicking on them with the mouse. (Shortcut: D)"), [self=this, ui=ui]()
         {
            selection_t sel = ui->find_selection_under_mouse(selection_type_t::tile);
            ui->delete_polygon(sel);
         });

         delete_poly_toggle = CreateToggle(L::t(L"Delete Polygon"), icons.delete_polygon, {}, L::t(L"Delete polygons by clicking on them with the mouse. (Shortcut: D)"), [self = this, ui = ui]()
         {
            ui->update_mouse_mode(self->delete_poly_toggle, mouse_mode_t::delete_polygon);
         });

         toggle_inclusion_action = CreateAction(L::t(L"Include Polygon"), icons.toggle_inclusion, 'T', L::t(L"Toggle the inclusion of polygons in the tiling by clicking on them with the mouse. (Shortcut: T)"), [self=this, ui=ui]()
         {
            selection_t sel = ui->find_selection_under_mouse(selection_type_t::tile);
            ui->toggle_inclusion(sel);
         });

         toggle_inclusion_toggle = CreateToggle(L::t(L"Include Polygon"), icons.toggle_inclusion, {}, L::t(L"Toggle the inclusion of polygons in the tiling by clicking on them with the mouse. (Shortcut: T)"), [self = this, ui = ui]()
         {
            ui->update_mouse_mode(self->toggle_inclusion_toggle, mouse_mode_t::include_polygon);
         });

         pan_toggle = CreateToggle(L::t(L"Pan"), icons.canvas_translate, {}, L::t(L"Pan the view by drag-and-drop with the mouse. (<Shift> + left mouse button.)"), [self=this, ui=ui]()
         {
            ui->update_mouse_mode(self->pan_toggle, mouse_mode_t::pan_view);
         });

         rotate_toggle = CreateToggle(L::t(L"Rotate"), icons.canvas_rotate, {}, L::t(L"Rotate the view by drag-and-drop with the mouse. (<Shift> + middle mouse button.)"), [self=this, ui=ui]()
         {
            ui->update_mouse_mode(self->rotate_toggle, mouse_mode_t::rotate_view);
         });

         zoom_toggle = CreateToggle(L::t(L"Zoom"), icons.canvas_zoom, {}, L::t(L"Zoom the view by drag-and-drop with the mouse. (<Shift> + right mouse button.)"), [self=this, ui=ui]()
         {
            ui->update_mouse_mode(self->zoom_toggle, mouse_mode_t::zoom_view);
         });

         for (int i = 0; i < 10; ++i)
         {
            number_actions[i] = CreateAction(QString::asprintf("%d", i), 0, '0' + i, nullptr, [ui=ui, i=i]()
            {
               ui->update_polygon_sides(i);
            });
            number_actions[i]->setShortcutContext(Qt::ShortcutContext::WindowShortcut);
            addAction(number_actions[i]);
         }

         QActionGroup * modif_group = new QActionGroup(this);
         modif_group->setExclusive(true);
         modif_group->addAction(draw_trans_toggle);
         modif_group->addAction(draw_poly_toggle);
         modif_group->addAction(copy_poly_toggle);
         modif_group->addAction(delete_poly_toggle);
         modif_group->addAction(move_poly_toggle);
         modif_group->addAction(toggle_inclusion_toggle);

         QActionGroup * mouse_group = new QActionGroup(this);
         mouse_group->setExclusive(true);
         mouse_group->addAction(pan_toggle);
         mouse_group->addAction(rotate_toggle);
         mouse_group->addAction(zoom_toggle);
      }

   }
}

// vim: sw=3 : sts=3 : et : sta : 

