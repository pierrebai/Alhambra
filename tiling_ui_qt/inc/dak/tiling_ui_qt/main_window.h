#pragma once

#ifndef DAK_TILING_UI_QT_MAIN_WINDOW_H
#define DAK_TILING_UI_QT_MAIN_WINDOW_H

#include <dak/tiling_ui_qt/styles_editor.h>
#include <dak/tiling_ui_qt/figure_editor.h>
#include <dak/tiling_ui_qt/figure_selector.h>
#include <dak/tiling_ui_qt/layers_selector.h>

#include <dak/ui_qt/layered_canvas.h>

#include <dak/tiling_style/known_mosaics_generator.h>

#include <dak/tiling/mosaic.h>
#include <dak/tiling/known_tilings.h>

#include <dak/geometry/rect.h>

#include <dak/utility/undo_stack.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qtoolbutton.h>

#include <vector>
#include <map>

namespace dak
{
   namespace tiling_ui_qt
   {
      typedef dak::tiling::mosaic mosaic;
      typedef dak::ui::layer layer;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Main window icon IDs.

      struct main_window_icons
      {
         int app_icon;
         int mosaic_previous;
         int mosaic_next;
         int undo;
         int redo;
         int mosaic_open;
         int mosaic_save;
         int export_img;
         int export_svg;
         int canvas_translate;
         int canvas_rotate;
         int canvas_zoom;
         int canvas_redraw;
         int layer_copy;
         int layer_add;
         int layer_delete;
         int layer_move_up;
         int layer_move_down;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // The main window of Alhambra.

      class main_window : public QMainWindow
      {
      public:
         // Create the main window.
         main_window(const main_window_icons& icons);

      protected:
         typedef std::map<std::shared_ptr<mosaic>, dak::geometry::map> calculated_mosaics;

         // Create the UI elements.
         void build_ui(const main_window_icons& icons);

         // Connect the signals of the UI elements.
         void connect_ui();

         // Fill the UI with the intial data.
         void fill_ui();

         // The redraw UI call-backs.
         geometry::rect window_filling_region();
         std::vector<std::shared_ptr<layer>> get_avail_layers();
         void update_layered_transform(const geometry::rect& bounds);
         const geometry::map& find_calculated_mosaic(calculated_mosaics& calc_mos, const std::shared_ptr<mosaic>& mosaic);
         void update_canvas_layers(const std::vector<std::shared_ptr<layer>>& layers);

         // The layers UI call-backs.
         std::vector<std::shared_ptr<layer>> get_selected_layers();
         std::vector<std::shared_ptr<tiling_style::style>> get_selected_styles();
         std::vector<std::shared_ptr<mosaic>> get_selected_mosaics();
         std::vector<std::shared_ptr<layer>> find_styles_layers(const std::vector<std::shared_ptr<tiling_style::style>>& styles);
         void update_layer_list();
         void fill_layer_list();

         // The figures list filling.
         std::vector<std::shared_ptr<figure>> get_all_avail_figures();
         std::vector<std::shared_ptr<figure>> get_merged_avail_figures();
         void fill_figure_list();
         std::shared_ptr<figure> get_selected_figure();
         void fill_figure_editor(bool force_update = false);

         // Undo / redo tool-bar buttons.
         void deaden_styled_mosaic(std::any& data);
         void update_undo_redo_actions();
         void awaken_styled_mosaic(const std::any& data);
         void awaken_to_empty_canvas();
         void clear_undo_stack();
         void commit_to_undo();

         // Layer manipulations.
         dak::ui::layered::layers clone_layers(const dak::ui::layered::layers& layers);
         void add_layer(const std::shared_ptr<mosaic>& new_mosaic);

         // The mosaic tool-bar buttons.
         void update_mosaic_map(const std::vector<std::shared_ptr<layer>>& layers, const std::wstring& name);

         // The canvas manipulation tool-bar buttons.
         void update_canvas_mode();

         // Data.
         std::vector<std::wstring> errors;
         dak::tiling::known_tilings known_tilings;
         dak::tiling_style::known_mosaics_generator mosaic_gen;
         dak::utility::undo_stack undo_stack;
         dak::ui::layered layered;

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
         layers_selector* layer_list = nullptr;

         styles_editor* styles_editor = nullptr;

         dak::tiling_ui_qt::figure_selector* figure_list = nullptr;
         figure_editor* figure_editor = nullptr;

         dak::ui_qt::layered_canvas* canvas = nullptr;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 