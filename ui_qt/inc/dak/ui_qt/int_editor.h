#pragma once

#ifndef DAK_UI_QT_INT_EDITOR_H
#define DAK_UI_QT_INT_EDITOR_H

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qspinbox>
#include <QtWidgets/qslider>
#include <QtWidgets/qlabel>
#include <QtWidgets/qwidget>

namespace dak
{
   namespace ui_qt
   {
      class int_editor : public QWidget
      {
      public:
         std::function<void(int)> value_changed_callback;

         int_editor(QWidget* parent, const wchar_t* label_text);
         int_editor(QWidget* parent, const wchar_t* label_text, int value);
         int_editor(QWidget* parent, const wchar_t* label_text, int value, std::function<void(int)> changed_callback);

         void set_limits(int min, int max);

         void set_value(int new_value, bool call_callback = false);

      private:
         void set_value_from_slider(int new_value);
         void set_value_from_spin_box(int new_value);

         void build_ui(const wchar_t* label_text);

         int current_value = INT_MIN;
         QHBoxLayout* layout = nullptr;
         QLabel* label = nullptr;
         QSlider* slider = nullptr;
         QSpinBox* text_editor = nullptr;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
