#pragma once

#ifndef DAK_TILING_UI_QT_FIGURE_SELECTOR_H
#define DAK_TILING_UI_QT_FIGURE_SELECTOR_H

#include <dak/tiling/figure.h>

#include <QtWidgets/qwidget>

#include <memory>
#include <functional>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling::figure;

      class figure_selector_ui;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select a figure.

      class figure_selector : public QWidget
      {
      public:
         // What is being edited.
         typedef std::vector<std::shared_ptr<figure>> figures;

         // Callback when the selection changed.
         typedef std::function<void(const std::shared_ptr<figure>&)> selection_changed_callback;
         selection_changed_callback selection_changed;

         // Callback when the figure type is changed.
         typedef std::function<void(const std::shared_ptr<figure>&)> figure_changed_callback;
         figure_changed_callback figure_changed;

         // Callback when the figure type is changed.
         typedef std::function<void(std::shared_ptr<figure> before, std::shared_ptr<figure> after)> figure_swapped_callback;
         figure_swapped_callback figure_swapped;

         // Create a figure selector with the given parent widget.
         figure_selector(QWidget* parent);

         // Set the figures being edited.
         void set_edited(const figures& edited);
         const figures& get_edited() const;

         // Retrieve the selected figure.
         std::shared_ptr<figure> get_selected_figure() const;

      protected:
         std::unique_ptr<figure_selector_ui> ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
