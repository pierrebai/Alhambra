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
      using mosaic_t = dak::tiling::mosaic_t;
      using styled_mosaic_t = dak::tiling_style::styled_mosaic_t;
      using layer_t = dak::ui::layer_t;
      using transform_t = geometry::transform_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Main window icon IDs.

      struct main_window_icons_t : tiling_editor_icons_t, layers_selector_icons_t
      {
         int mosaic_previous = 0;
         int mosaic_next = 0;

         int mosaic_new = 0;
         int mosaic_open = 0;
         int mosaic_save = 0;

         int export_img = 0;
         int export_svg = 0;

         int canvas_redraw = 0;

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
         struct calculated_mosaic_t
         {
            std::shared_ptr<mosaic_t> mosaic;
            transform_t trf;
            dak::geometry::edges_map_t edges_map;
         };
         typedef std::vector<calculated_mosaic_t> mosaic_edges_map_cache_t;

         // Add the tiling found in the given folder.
         void add_tilings_from(const std::wstring& folder);

         // Create the UI elements.
         void build_ui(const main_window_icons_t& icons);

         // Connect the signals of the UI elements.
         void connect_ui(const tiling_editor_icons_t& icons);

         // Fill the UI with the intial data.
         void fill_ui();

         // The redraw UI call-backs.
         geometry::rectangle_t window_filling_region(const std::shared_ptr<layer_t>& layer);
         std::vector<std::shared_ptr<styled_mosaic_t>> get_avail_mosaics();
         std::vector<std::shared_ptr<layer_t>> get_avail_layers();
         void update_layered_transform();
         const geometry::edges_map_t& find_calculated_mosaic(mosaic_edges_map_cache_t& calc_mos, const std::shared_ptr<styled_mosaic_t>& styled_mosaic);
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
         void update_mosaic_map(const std::vector<std::shared_ptr<styled_mosaic_t>>& mosaics, const std::wstring& name);
         void update_mosaic_map(const std::vector<std::shared_ptr<layer_t>>& layers, const std::wstring& name);

         // The canvas manipulation tool-bar buttons.
         void update_canvas_mode();

         // Closing and saving.
         void closeEvent(QCloseEvent* ev);
         bool save_if_required(const std::wstring& action, const std::wstring& actioning);
         bool save_mosaic();

         // Data.
         std::vector<std::wstring> my_errors;
         dak::tiling::known_tilings_t my_known_tilings;
         dak::tiling_style::known_mosaics_generator_t my_mosaic_gen;
         dak::utility::undo_stack_t my_undo_stack;
         std::shared_ptr<dak::ui::layered_t> my_layered;
         std::shared_ptr<dak::ui::layered_t> my_original_mosaic;


         // UI elements.
         QAction* my_previous_mosaic_action = nullptr;
         QToolButton* my_previous_mosaic_button = nullptr;

         QAction* my_next_mosaic_action = nullptr;
         QToolButton* my_next_mosaic_button = nullptr;

         QAction* my_undo_action = nullptr;
         QToolButton* my_undo_button = nullptr;

         QAction* my_redo_action = nullptr;
         QToolButton* my_redo_button = nullptr;

         QAction* my_new_mosaic_action = nullptr;
         QToolButton* my_new_mosaic_button = nullptr;

         QAction* my_load_mosaic_action = nullptr;
         QToolButton* my_load_mosaic_button = nullptr;

         QAction* my_save_mosaic_action = nullptr;
         QToolButton* my_save_mosaic_button = nullptr;

         QAction* my_tiling_editor_action = nullptr;
         QToolButton* my_tiling_editor_button = nullptr;

         QAction* my_export_image_action = nullptr;
         QToolButton* my_export_image_button = nullptr;

         QAction* my_export_svg_action = nullptr;
         QToolButton* my_export_svg_button = nullptr;

         QAction* my_translate_action = nullptr;
         QToolButton* my_translate_button = nullptr;

         QAction* my_rotate_action = nullptr;
         QToolButton* my_rotate_button = nullptr;

         QAction* my_scale_action = nullptr;
         QToolButton* my_scale_button = nullptr;

         QAction* my_redraw_action = nullptr;
         QToolButton* my_redraw_button = nullptr;

         QDockWidget* my_layers_dock = nullptr;
         layers_selector_t* my_layer_list = nullptr;

         styles_editor_t* my_styles_editor = nullptr;

         dak::tiling_ui_qt::figure_selector_t* my_figure_list = nullptr;
         figure_editor_t* my_figure_editor = nullptr;

         dak::ui::qt::layered_canvas_t* my_layered_canvas = nullptr;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
