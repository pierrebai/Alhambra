#pragma once

#ifndef DAK_TILING_UI_QT_LAYERS_SELECTOR_H
#define DAK_TILING_UI_QT_LAYERS_SELECTOR_H

#include <dak/tiling/mosaic.h>

#include <dak/tiling_style/style.h>
#include <dak/tiling_style/styled_mosaic.h>

#include <dak/ui/layer.h>
#include <dak/ui/color.h>

#include <QtWidgets/qwidget>

#include <memory>
#include <functional>

namespace dak
{
   namespace tiling_ui_qt
   {
      using ui::layer_t;
      using tiling::mosaic_t;
      using tiling_style::style_t;
      using tiling_style::styled_mosaic_t;

      class layers_selector_ui_t;

      // Create an icon for the given mosaic.
      QIcon get_icon(const std::shared_ptr<styled_mosaic_t>& sm, int w, int h);
      QIcon get_icon(const std::shared_ptr<mosaic_t>& mosaic, const ui::color_t& co, int w, int h);

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order layers.

      class layers_selector_t : public QWidget
      {
      public:
         // What is being edited.
         typedef std::vector<std::shared_ptr<layer_t>> layers;
         typedef std::vector<std::shared_ptr<tiling_style::style_t>> styles;

         // Callback when the selection changed.
         typedef std::function<void(const layers&)> selection_changed_callback;
         selection_changed_callback selection_changed;

         // Callback when the edited styles are changed.
         typedef std::function<void(const layers&)> layers_changed_callback;
         layers_changed_callback layers_changed;

         // Callback when a new layer_t should be added.
         typedef std::function<void()> new_layer_requested_callback;
         new_layer_requested_callback new_layer_requested;

         // Create a layer_t selector with the given parent widget and layers.
         layers_selector_t(QWidget* parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon);

         // Set the layers being edited.
         void set_edited(const layers& edited);
         const layers& get_edited() const;

         // Update the list UI to represent changes made to layers.
         void update_list_content();

         // Retrieve the selected layers or styles.
         layers get_selected_layers() const;
         styles get_selected_styles() const;

      protected:
         std::unique_ptr<layers_selector_ui_t> ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
