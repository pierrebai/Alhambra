#pragma once

#ifndef DAK_TILING_UI_QT_TILING_WINDOW_H
#define DAK_TILING_UI_QT_TILING_WINDOW_H

#include <dak/tiling_ui_qt/tiling_editor.h>

#include <dak/tiling/tiling.h>

#include <QtWidgets/qwidget>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qdockwidget.h>

#include <memory>
#include <functional>
#include <experimental/filesystem>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling::tiling;
      typedef std::experimental::filesystem::path file_path;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Used to design the tilings that are used as skeletons for the Islamic
      // construction process.

      class tiling_window : public QMainWindow
      {
      public:
         // Creation.
         tiling_window(const tiling_editor_icons& icons, QWidget *parent);

      private:
         void build_actions(const tiling_editor_icons& icons);
         void build_ui(const tiling_editor_icons& icons);

         void closeEvent(QCloseEvent* ev);

         // Action callbacks.
         void new_tiling();
         void open_tiling();
         void select_tiling();
         bool save_tiling();

         void set_tiling(const tiling& tiling, const file_path& file);
         tiling create_tiling_from_data(const file_path& file);
         bool has_original_data_changed();
         bool save_if_required(const std::wstring& action, const std::wstring& actioning);

         // Tiling being designed: all features and those actually in the tiling.
         file_path      original_file;
         tiling         original_tiling;

         tiling_editor* editor = nullptr;

         QToolButton* new_action = nullptr;
         QToolButton* open_action = nullptr;
         QToolButton* select_action = nullptr;
         QToolButton* save_action = nullptr;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
