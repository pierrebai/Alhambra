#pragma once

#ifndef DAK_TILING_UI_QT_ASK_H
#define DAK_TILING_UI_QT_ASK_H

#include <memory>
#include <vector>

#include <filesystem>

class QWidget;

namespace dak
{
   namespace tiling_style
   {
      class styled_mosaic_t;
   }

   namespace tiling
   {
      class tiling_t;
      using known_tilings_t = std::vector<tiling_t>;
   }

   namespace tiling_ui_qt
   {
      using dak::tiling_style::styled_mosaic_t;
      using dak::tiling::tiling_t;
      using dak::tiling::known_tilings_t;

      // Show a dialog to open or save something.
      std::filesystem::path ask_open(const wchar_t* title, const wchar_t* file_types, QWidget* parent);
      std::filesystem::path ask_save(const wchar_t* title, const wchar_t* file_types, QWidget* parent);

      // Show a dialog to open or save a tiling.
      // The given path is updated with the selected one, if any.
      tiling_t ask_open_tiling(std::filesystem::path& path, QWidget* parent);
      bool ask_save_tiling(tiling_t& tiling, std::filesystem::path& path, QWidget* parent);

      // Show a dialog to open or save a layered mosaic.
      // The given path is updated with the selected one, if any.
      std::vector<std::shared_ptr<styled_mosaic_t>> ask_open_layered_mosaic(const known_tilings_t& knowns, std::filesystem::path& path, QWidget* parent);
      bool ask_save_layered_mosaic(const std::vector<std::shared_ptr<styled_mosaic_t>>& layers, std::filesystem::path& path, QWidget* parent);

      // Ask yes/no/cancel.
      enum class yes_no_cancel_t
      {
         no = 0,
         yes = 1,
         cancel = 2,
      };

      yes_no_cancel_t ask_yes_no_cancel(const std::wstring& title, const std::wstring& text, QWidget* parent);
      yes_no_cancel_t ask_yes_no_cancel(const wchar_t* title, const wchar_t* text, QWidget* parent);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
