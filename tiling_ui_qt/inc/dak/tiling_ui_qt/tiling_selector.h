#pragma once

#ifndef DAK_TILING_UI_QT_TILING_SELECTOR_H
#define DAK_TILING_UI_QT_TILING_SELECTOR_H

#include <dak/tiling/mosaic.h>

#include <QtWidgets/qdialog>

#include <memory>
#include <functional>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling::mosaic;

      class tiling_selector_ui;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select a tiling.

      class tiling_selector : public QDialog
      {
      public:
         // Callback when the tiling has been chosen.
         typedef std::function<void(const std::shared_ptr<mosaic>&)> tiling_chosen_callback;
         tiling_chosen_callback tiling_chosen;

         // Create a tiling selector with the given parent widget.
         tiling_selector(QWidget* parent);
         tiling_selector(QWidget* parent, tiling_chosen_callback tc);

         std::shared_ptr<mosaic> get_selected() const;

      protected:
         std::unique_ptr<tiling_selector_ui> ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
