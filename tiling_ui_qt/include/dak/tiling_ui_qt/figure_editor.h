#pragma once

#ifndef DAK_TILING_UI_QT_FIGURE_EDITOR_H
#define DAK_TILING_UI_QT_FIGURE_EDITOR_H

#include <dak/tiling/figure.h>

#include <QtWidgets/qwidget>

#include <memory>
#include <functional>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling::figure_t;

      class figure_editor_ui_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a figure.

      class figure_editor_t : public QWidget
      {
      public:
         typedef std::function<void(std::shared_ptr<figure_t>, bool interacting)> figure_changed_callback;
         figure_changed_callback figure_changed;

         // Create a figure editor with the given parent widget and figure.
         figure_editor_t(QWidget* parent);
         figure_editor_t(QWidget* parent, figure_changed_callback figure_changed);
         figure_editor_t(QWidget* parent, std::shared_ptr<figure_t> edited, figure_changed_callback figure_changed);

         // Set the figures being edited.
         void set_edited(std::shared_ptr<figure_t> edited, bool force_ui_update = false);
         std::shared_ptr<figure_t> get_edited() const;

      protected:
         std::unique_ptr<figure_editor_ui_t> ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
