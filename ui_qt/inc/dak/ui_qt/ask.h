#pragma once

#ifndef DAK_UI_QT_IO_UTILITY_H
#define DAK_UI_QT_IO_UTILITY_H

#include <memory>
#include <vector>

#include <experimental/filesystem>

class QWidget;

namespace dak
{
   namespace ui
   {
      class layer;
   }

   namespace tiling
   {
      class tiling;
      class known_tilings;
   }

   namespace ui_qt
   {
      using dak::ui::layer;
      using dak::tiling::tiling;
      using dak::tiling::known_tilings;

      // Show a dialog to open or save something.
      std::experimental::filesystem::path ask_open(const wchar_t* title, const wchar_t* file_types, QWidget* parent);
      std::experimental::filesystem::path ask_save(const wchar_t* title, const wchar_t* file_types, QWidget* parent);

      // Show a dialog to open or save a tiling.
      // The given path is updated with the selected one, if any.
      tiling ask_open_tiling(std::experimental::filesystem::path& path, QWidget* parent);
      bool ask_save_tiling(const tiling& tiling, std::experimental::filesystem::path& path, QWidget* parent);

      // Show a dialog to open or save a layered mosaic.
      // The given path is updated with the selected one, if any.
      std::vector<std::shared_ptr<ui::layer>> ask_open_layered_mosaic(const known_tilings& knowns, std::experimental::filesystem::path& path, QWidget* parent);
      bool ask_save_layered_mosaic(const std::vector<std::shared_ptr<ui::layer>>& layers, std::experimental::filesystem::path& path, QWidget* parent);

      // Ask yes/no/cancel.
      enum class yes_no_cancel
      {
         no = 0,
         yes = 1,
         cancel = 2,
      };

      yes_no_cancel ask_yes_no_cancel(const std::wstring& title, const std::wstring& text, QWidget* parent);
      yes_no_cancel ask_yes_no_cancel(const wchar_t* title, const wchar_t* text, QWidget* parent);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
