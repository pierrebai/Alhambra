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
      using tiling_style::style_t;

      class styles_editor_ui_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit multiple styles.

      class styles_editor_t : public QWidget
      {
      public:
         typedef dak::tiling_style::style_t style_t;

         // What is being edited.
         typedef std::vector<std::shared_ptr<style_t>> styles_t;

         // Callback when the editer styles are changed.
         typedef std::function<void(const styles_t&, bool interacting)> styles_changed_callback;
         styles_changed_callback styles_changed;

         // Create a style editor with the given parent widget and style.
         styles_editor_t(QWidget* parent);
         styles_editor_t(QWidget* parent, styles_changed_callback style_changed);
         styles_editor_t(QWidget* parent, const styles& edited, styles_changed_callback style_changed);

         void set_edited(const styles_t& edited);
         const styles_t& get_edited() const;

      protected:
         std::unique_ptr<styles_editor_ui_t> ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
