#include <dak/ui_qt/int_editor.h>

namespace dak
{
   namespace ui_qt
   {
      int_editor::int_editor(QWidget* parent, const wchar_t* label_text)
      : int_editor(parent, label_text, 0, nullptr)
      {
      }

      int_editor::int_editor(QWidget* parent, const wchar_t* label_text, int value)
      : int_editor(parent, label_text, value, nullptr)
      {
      }

      int_editor::int_editor(QWidget* parent, const wchar_t* label_text, int value, value_changed_callback changed_callback)
         : QWidget(parent), current_value(~value), value_changed(changed_callback)
      {
         build_ui(label_text);
         set_value(value);
      }

      void int_editor::set_limits(int min, int max)
      {
         slider->setRange(min, max);
         text_editor->setRange(min, max);
      }

      void int_editor::set_value(int new_value, bool call_callback)
      {
         if (current_value == new_value)
            return;

         auto prev_callback = value_changed;
         if (!call_callback)
            value_changed = nullptr;

         current_value = new_value;

         if (text_editor->value() != new_value)
            text_editor->setValue(new_value);

         if (slider->value() != new_value)
            slider->setValue(new_value);

         if (call_callback && value_changed)
            value_changed(new_value, false);

         if (!call_callback)
            value_changed = prev_callback;
      }

      void int_editor::set_value_from_spin_box(int new_value)
      {
         if (current_value == new_value)
            return;

         current_value = new_value;

         if (slider->value() != new_value)
            slider->setValue(new_value);

         if (value_changed)
            value_changed(new_value, false);
      }

      void int_editor::set_value_from_slider(int new_value, bool force_update)
      {
         if (!force_update && current_value == new_value)
            return;

         current_value = new_value;

         if (text_editor->value() != new_value)
            text_editor->setValue(new_value);

         if (value_changed)
            value_changed(new_value, slider->isSliderDown());
      }

      void int_editor::set_value_from_slider(int new_value)
      {
         set_value_from_slider(new_value, false);
      }

      void int_editor::slider_pressed()
      {
         start_value = current_value;
      }

      void int_editor::slider_released()
      {
         set_value_from_slider(slider->value(), start_value != current_value);
      }

      void int_editor::build_ui(const wchar_t* label_text)
      {
         label = new QLabel(QString::fromWCharArray(label_text), this);
         slider = new QSlider(Qt::Orientation::Horizontal, this);
         text_editor = new QSpinBox(this);

         layout = new QHBoxLayout(this);
         layout->setContentsMargins(0, 0, 0, 0);
         layout->addWidget(label);
         layout->addWidget(slider);
         layout->addWidget(text_editor);

         slider->setRange(0, 30);
         text_editor->setRange(0, 30);
         text_editor->setSingleStep(1);

         slider->connect(slider, &QSlider::valueChanged, [self=this](int new_value) { self->set_value_from_slider(new_value); });
         slider->connect(slider, &QSlider::sliderPressed, this, &int_editor::slider_pressed);
         slider->connect(slider, &QSlider::sliderReleased, this, &int_editor::slider_released);

         void (QSpinBox::*vc)(int) = &QSpinBox::valueChanged;
         text_editor->connect(text_editor, vc, this, &int_editor::set_value_from_spin_box);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
