#pragma once

#ifndef DAK_TILING_UI_QT_TILING_SELECTOR_H
#define DAK_TILING_UI_QT_TILING_SELECTOR_H

#include <dak/tiling/mosaic.h>
#include <dak/tiling/known_tilings.h>

#include <QtWidgets/qdialog>

#include <memory>
#include <functional>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling::tiling_t;
      using dak::tiling::known_tilings_t;

      class tiling_selector_ui_t;
      struct tiling_editor_icons_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select a tiling.

      class tiling_selector_t : public QDialog
      {
      public:
         // Callback when the tiling has been chosen.
         typedef std::function<void(const std::shared_ptr<tiling_t>&)> tiling_chosen_callback;
         tiling_chosen_callback tiling_chosen;

         // Create a tiling selector with the given parent widget.
         tiling_selector_t(known_tilings_t& known_tilings, const tiling_editor_icons_t& icons, QWidget* parent);
         tiling_selector_t(known_tilings_t& known_tilings, const tiling_editor_icons_t& icons, QWidget* parent, tiling_chosen_callback tc);

         std::shared_ptr<tiling_t> get_selected() const;

      protected:
         std::unique_ptr<tiling_selector_ui_t> my_ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
