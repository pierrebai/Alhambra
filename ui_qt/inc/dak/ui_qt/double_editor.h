#pragma once

#ifndef DAK_UI_QT_DOUBLE_EDITOR_H
#define DAK_UI_QT_DOUBLE_EDITOR_H

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qdoublespinbox>
#include <QtWidgets/qslider>
#include <QtWidgets/qlabel>
#include <QtWidgets/qwidget>

#include <cmath>

namespace dak
{
   namespace ui_qt
   {
      class double_editor : public QWidget
      {
      public:
         std::function<void(double)> value_changed_callback;

         double_editor(QWidget* parent, const wchar_t* label_text);
         double_editor(QWidget* parent, const wchar_t* label_text, double value);
         double_editor(QWidget* parent, const wchar_t* label_text, double value, std::function<void(double)> changed_callback);

         void set_limits(double min, double max, double step);

         void set_value(double new_value, bool call_callback = false);

      private:
         void set_value_from_slider(int new_value);
         void set_value_from_spin_box(double new_value);

         void build_ui(const wchar_t* label_text);

         double current_value = NAN;
         QHBoxLayout* layout = nullptr;
         QLabel* label = nullptr;
         QSlider* slider = nullptr;
         QDoubleSpinBox* text_editor = nullptr;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
