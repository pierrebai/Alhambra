#pragma once

#ifndef DAK_TILING_UI_QT_MAIN_WINDOW_H
#define DAK_TILING_UI_QT_MAIN_WINDOW_H

#include <dak/tiling_ui_qt/styles_editor.h>
#include <dak/tiling_ui_qt/figure_editor.h>
#include <dak/tiling_ui_qt/figure_selector.h>
#include <dak/tiling_ui_qt/layers_selector.h>
#include <dak/tiling_ui_qt/tiling_editor.h>

#include <dak/ui/qt/layered_canvas.h>

#include <dak/tiling_style/known_mosaics_generator.h>

#include <dak/tiling/mosaic.h>
#include <dak/tiling/known_tilings.h>

#include <dak/geometry/rectangle.h>

#include <dak/utility/undo_stack.h>

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qtoolbutton.h>

#include <vector>
#include <map>

namespace dak
{
   namespace tiling_ui_qt
   {
      typedef dak::tiling::mosaic_t mosaic_t;
      typedef dak::ui::layer_t layer_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Main window icon IDs.

      struct main_window_icons_t : tiling_editor_icons_t
      {
         int mosaic_previous = 0;
         int mosaic_next = 0;

         int mosaic_open = 0;
         int mosaic_save = 0;

         int export_img = 0;
         int export_svg = 0;

         int canvas_redraw = 0;

         int layer_copy = 0;
         int layer_add = 0;
         int layer_delete = 0;
         int layer_move_up = 0;
         int layer_move_down = 0;

         int tiling_editor = 0;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // The main window of Alhambra.

      class main_window_t : public QMainWindow
      {
      public:
         // Create the main window.
         main_window_t(const main_window_icons_t& icons);

      protected:
         typedef std::map<std::shared_ptr<mosaic_t>, dak::geometry::edges_map_t> calculated_mosaics;

         // Add the tiling found in the given folder.
         void add_tilings_from(const std::wstring& folder);

         // Create the UI elements.
         void build_ui(const main_window_icons_t& icons);

         // Connect the signals of the UI elements.
         void connect_ui(const tiling_editor_icons_t& icons);

         // Fill the UI with the intial data.
         void fill_ui();

         // The redraw UI call-backs.
         geometry::rectangle_t window_filling_region();
         std::vector<std::shared_ptr<layer_t>> get_avail_layers();
         void update_layered_transform(const geometry::rectangle_t& bounds);
         const geometry::edges_map_t& find_calculated_mosaic(calculated_mosaics& calc_mos, const std::shared_ptr<mosaic_t>& mosaic);
         void update_canvas_layers(const std::vector<std::shared_ptr<layer_t>>& layers);

         // The layers UI call-backs.
         std::vector<std::shared_ptr<layer_t>> get_selected_layers();
         std::vector<std::shared_ptr<tiling_style::style_t>> get_selected_styles();
         std::vector<std::shared_ptr<mosaic_t>> get_selected_mosaics();
         std::vector<std::shared_ptr<layer_t>> find_styles_layers(const std::vector<std::shared_ptr<tiling_style::style_t>>& styles);
         void update_layer_list();
         void fill_layer_list();

         // The figures list filling.
         std::vector<std::shared_ptr<figure_t>> get_all_avail_figures();
         std::vector<std::shared_ptr<figure_t>> get_merged_avail_figures();
         void fill_figure_list();
         std::shared_ptr<figure_t> get_selected_figure();
         void fill_figure_editor(bool force_update = false);

         // Undo / redo tool-bar buttons.
         void deaden_styled_mosaic(std::any& data);
         void update_undo_redo_actions();
         void awaken_styled_mosaic(const std::any& data);
         void awaken_to_empty_canvas();
         void clear_undo_stack();
         void commit_to_undo();

         // Layer manipulations.
         dak::ui::layered_t::layers_t clone_layers(const dak::ui::layered_t::layers_t& layers);
         void add_layer(const std::shared_ptr<mosaic_t>& new_mosaic);

         // The mosaic tool-bar buttons.
         void update_mosaic_map(const std::vector<std::shared_ptr<layer_t>>& layers, const std::wstring& name);

         // The canvas manipulation tool-bar buttons.
         void update_canvas_mode();

         // Closing and saving.
         void closeEvent(QCloseEvent* ev);
         bool save_if_required(const std::wstring& action, const std::wstring& actioning);
         bool save_mosaic();

         // Data.
         std::vector<std::wstring> errors;
         dak::tiling::known_tilings_t known_tilings;
         dak::tiling_style::known_mosaics_generator_t mosaic_gen;
         dak::utility::undo_stack_t undo_stack;
         dak::ui::layered_t layered;

         // UI elements.
         QAction* previous_mosaic_action = nullptr;
         QToolButton* previous_mosaic_button = nullptr;

         QAction* next_mosaic_action = nullptr;
         QToolButton* next_mosaic_button = nullptr;

         QAction* undo_action = nullptr;
         QToolButton* undo_button = nullptr;

         QAction* redo_action = nullptr;
         QToolButton* redo_button = nullptr;

         QAction* load_mosaic_action = nullptr;
         QToolButton* load_mosaic_button = nullptr;

         QAction* save_mosaic_action = nullptr;
         QToolButton* save_mosaic_button = nullptr;

         QAction* tiling_editor_action = nullptr;
         QToolButton* tiling_editor_button = nullptr;

         QAction* export_image_action = nullptr;
         QToolButton* export_image_button = nullptr;

         QAction* export_svg_action = nullptr;
         QToolButton* export_svg_button = nullptr;

         QAction* translate_action = nullptr;
         QToolButton* translate_button = nullptr;

         QAction* rotate_action = nullptr;
         QToolButton* rotate_button = nullptr;

         QAction* scale_action = nullptr;
         QToolButton* scale_button = nullptr;

         QAction* redraw_action = nullptr;
         QToolButton* redraw_button = nullptr;

         QDockWidget* layers_dock = nullptr;
         layers_selector_t* layer_list = nullptr;

         styles_editor_t* styles_editor = nullptr;

         dak::tiling_ui_qt::figure_selector_t* figure_list = nullptr;
         figure_editor_t* figure_editor = nullptr;

         dak::ui::qt::layered_canvas_t* canvas = nullptr;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
