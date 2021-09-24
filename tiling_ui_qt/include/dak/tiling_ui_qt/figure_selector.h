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
      using tiling::figure_t;

      class figure_selector_ui_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select a figure.

      class figure_selector_t : public QWidget
      {
      public:
         // What is being edited.
         typedef std::vector<std::shared_ptr<figure_t>> figures;

         // Callback when the selection changed.
         typedef std::function<void(const std::shared_ptr<figure_t>&)> selection_changed_callback;
         selection_changed_callback selection_changed;

         // Callback when the figure type is changed.
         typedef std::function<void(const std::shared_ptr<figure_t>&)> figure_changed_callback;
         figure_changed_callback figure_changed;

         // Callback when the figure type is changed.
         typedef std::function<void(std::shared_ptr<figure_t> before, std::shared_ptr<figure_t> after)> figure_swapped_callback;
         figure_swapped_callback figure_swapped;

         // Create a figure selector with the given parent widget.
         figure_selector_t(QWidget* parent);

         // Set the figures being edited.
         void set_edited(const figures& edited);
         const figures& get_edited() const;

         // Retrieve the selected figure.
         std::shared_ptr<figure_t> get_selected_figure() const;

      protected:
         std::unique_ptr<figure_selector_ui_t> my_ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
