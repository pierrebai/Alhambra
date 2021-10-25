#pragma once

#ifndef DAK_TILING_UI_QT_TILING_EDITOR_H
#define DAK_TILING_UI_QT_TILING_EDITOR_H

#include <dak/tiling/tiling.h>

#include <dak/ui/color.h>
#include <dak/ui/mouse.h>
#include <dak/ui/drawing.h>
#include <dak/ui/qt/message_reporter.h>

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qaction.h>

#include <memory>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling::tiling_t;
      using dak::ui::drawing_t;
      using dak::ui::qt::message_reporter_t;

      class tiling_editor_ui_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Tiling editor icon IDs.

      struct tiling_editor_icons_t
      {
         int app_icon = 0;

         int undo = 0;
         int redo = 0;

         int canvas_translate = 0;
         int canvas_rotate = 0;
         int canvas_zoom = 0;

         int tiling_new = 0;
         int tiling_open = 0;
         int tiling_save = 0;

         int draw_translation = 0;
         int draw_inflation = 0;
         int clear_translation = 0;
         int fill_with_translation = 0;

         int add_polygon = 0;
         int draw_polygon = 0;
         int copy_polygon = 0;
         int move_polygon = 0;
         int delete_polygon = 0;

         int toggle_inclusion = 0;
         int remove_excluded = 0;
         int exclude_all = 0;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // It's used to design the tilings that are used as skeletons for the
      // mosaics.

      class tiling_editor_t : public QWidget
      {
      public:
         // End-user actions.
         QAction*  my_draw_trans_toggle = nullptr;
         QAction*  my_draw_inflation_toggle = nullptr;
         QAction*  my_clear_trans_action = nullptr;
         QAction*  my_fill_trans_action = nullptr;

         QAction*  my_add_poly_action = nullptr;
         QAction*  my_draw_poly_toggle = nullptr;
         QAction*  my_copy_poly_toggle = nullptr;
         QAction*  my_move_poly_toggle = nullptr;
         QAction*  my_delete_poly_toggle = nullptr;

         QAction*  my_toggle_inclusion_toggle = nullptr;
         QAction*  my_remove_excluded_action = nullptr;
         QAction*  my_exclude_all_action = nullptr;

         QAction*  my_pan_toggle = nullptr;
         QAction*  my_rotate_toggle = nullptr;
         QAction*  my_zoom_toggle = nullptr;

         QAction*  number_actions[10] = { };

         QAction*  my_copy_poly_action = nullptr;
         QAction*  my_delete_poly_action = nullptr;
         QAction*  my_toggle_inclusion_action = nullptr;

         // Creation.
         tiling_editor_t(const tiling_editor_icons_t& icons, QWidget* parent);

         // Tiling management.
         void set_tiling(const std::shared_ptr<tiling_t>& tiling);
         std::shared_ptr<tiling_t> create_tiling();
         bool verify_tiling(const std::wstring& operation);

         // Error reporting.
         void report_error(const std::wstring& text, message_reporter_t::category_t cat = message_reporter_t::category_t::error);

      private:
         // End-user actions.
         void build_actions(const tiling_editor_icons_t& icons);
         void build_ui();

         std::shared_ptr<tiling_editor_ui_t> my_ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

