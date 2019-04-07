#pragma once

#ifndef DAK_TILING_UI_QT_STYLES_EDITOR_H
#define DAK_TILING_UI_QT_STYLES_EDITOR_H

#include <dak/tiling_style/style.h>

#include <QtWidgets/qwidget>

#include <memory>
#include <functional>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling_style::style;

      class styles_editor_ui;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit multiple styles.

      class styles_editor : public QWidget
      {
      public:
         typedef dak::tiling_style::style style;

         // What is being edited.
         typedef std::vector<std::shared_ptr<style>> styles;

         // Callback when the editer styles are changed.
         typedef std::function<void(const styles&, bool interacting)> styles_changed_callback;
         styles_changed_callback styles_changed;

         // Create a style editor with the given parent widget and style.
         styles_editor(QWidget* parent);
         styles_editor(QWidget* parent, styles_changed_callback style_changed);
         styles_editor(QWidget* parent, const styles& edited, styles_changed_callback style_changed);

         void set_edited(const styles& edited);
         const styles& get_edited() const;

      protected:
         std::unique_ptr<styles_editor_ui> ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
