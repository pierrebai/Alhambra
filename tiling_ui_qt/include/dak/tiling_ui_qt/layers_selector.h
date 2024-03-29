#pragma once

#ifndef DAK_TILING_UI_QT_LAYERS_SELECTOR_H
#define DAK_TILING_UI_QT_LAYERS_SELECTOR_H

#include <dak/tiling/mosaic.h>

#include <dak/tiling_style/style.h>
#include <dak/tiling_style/styled_mosaic.h>

#include <dak/ui/layer.h>
#include <dak/ui/color.h>

#include <QtWidgets/qwidget>

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

      ////////////////////////////////////////////////////////////////////////////
      //
      // Create an icon for the given mosaic.
      QIcon get_icon(const std::shared_ptr<styled_mosaic_t>& sm, int w, int h);
      QIcon get_icon(const std::shared_ptr<mosaic_t>& mosaic, const ui::color_t& co, int w, int h);

      ////////////////////////////////////////////////////////////////////////////
      //
      // Layers selector icon IDs.

      struct layers_selector_icons_t
      {
         int layer_copy = 0;
         int layer_add = 0;
         int layer_delete = 0;
         int layer_copy_position = 0;
         int layer_move_up = 0;
         int layer_move_down = 0;
      };

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

         // Callback when a new layer should be added.
         typedef std::function<void()> new_layer_requested_callback;
         new_layer_requested_callback new_layer_requested;

         // Create a layer selector with the given parent widget and layers.
         layers_selector_t(QWidget* parent, const layers_selector_icons_t& icons);

         // Set the layers being edited.
         void set_edited(const layers& edited);
         const layers& get_edited() const;

         // Update the list UI to represent changes made to layers.
         void update_list_content();

         // Retrieve the selected layers or styles.
         layers get_selected_layers() const;
         styles get_selected_styles() const;

      protected:
         layers_selector_ui_t* my_ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
