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
#include <filesystem>

namespace dak
{
   namespace tiling
   {
      class tiling_t;
      using known_tilings_t = std::vector<tiling_t>;
   }

   namespace tiling_ui_qt
   {
      using dak::tiling::tiling_t;
      using dak::tiling::known_tilings_t;
      typedef std::filesystem::path file_path_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Used to design the tilings that are used as skeletons for the Islamic
      // construction process.

      class tiling_window_t : public QMainWindow
      {
      public:
         // Creation.
         tiling_window_t(known_tilings_t& known_tilings, const tiling_editor_icons_t& icons, QWidget *parent);

      private:
         void build_actions(const tiling_editor_icons_t& icons);
         void build_ui(const tiling_editor_icons_t& icons);

         void closeEvent(QCloseEvent* ev);

         // Action callbacks.
         void new_tiling();
         void open_tiling();
         void select_tiling(const tiling_editor_icons_t& icons);
         bool save_tiling();

         void set_tiling(const tiling_t& tiling, const file_path_t& file);
         tiling_t create_tiling_from_data(const file_path_t& file);
         bool has_original_data_changed();
         bool save_if_required(const std::wstring& action, const std::wstring& actioning);

         // Tiling being designed: all features and those actually in the tiling.
         file_path_t      original_file;
         tiling_t         original_tiling;

         tiling_editor_t* editor = nullptr;

         known_tilings_t& known_tilings;

         QToolButton* new_action = nullptr;
         QToolButton* open_action = nullptr;
         QToolButton* select_action = nullptr;
         QToolButton* save_action = nullptr;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
