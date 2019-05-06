#pragma once

#ifndef DAK_TILING_UI_QT_TILING_EDITOR_H
#define DAK_TILING_UI_QT_TILING_EDITOR_H

#include <dak/tiling/tiling.h>

#include <dak/ui/color.h>
#include <dak/ui/mouse.h>
#include <dak/ui/drawing.h>
#include <dak/ui_qt/message_reporter.h>

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qaction.h>

#include <memory>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling::tiling;
      using dak::ui::drawing;
      using dak::ui_qt::message_reporter;

      class tiling_editor_ui;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Tiling editor icon IDs.

      struct tiling_editor_icons
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
      // It's used to design the tilings that are used as skeletons for the Islamic
      // construction process.  It's fairly featureful, much more rapid and accurate
      // than expressing the tilings directly as code, which is what I did in a
      // previous version.

      class tiling_editor : public QWidget
      {
      public:
         // End-user actions.
         QAction*  trans_action = nullptr;
         QAction*  clear_trans_action = nullptr;
         QAction*  fill_trans_action = nullptr;

         QAction*  add_poly_action = nullptr;
         QAction*  draw_poly_action = nullptr;
         QAction*  copy_poly_action = nullptr;
         QAction*  move_poly_action = nullptr;
         QAction*  delete_poly_action = nullptr;

         QAction*  toggle_inclusion_action = nullptr;
         QAction*  remove_excluded_action = nullptr;
         QAction*  exclude_all_action = nullptr;

         QAction*  pan_action = nullptr;
         QAction*  rotate_action = nullptr;
         QAction*  zoom_action = nullptr;

         QAction*  number_actions[10] = { };

         // Creation.
         tiling_editor(const tiling_editor_icons& icons, QWidget* parent);

         // Tiling management.
         void set_tiling(const tiling& tiling);
         tiling create_tiling();
         bool verify_tiling(const std::wstring& operation);

         // Error reporting.
         void report_error(const std::wstring& text, message_reporter::category cat = message_reporter::category::error);

      private:
         // End-user actions.
         void build_actions(const tiling_editor_icons& icons);
         void build_ui();

         std::shared_ptr<tiling_editor_ui> ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

