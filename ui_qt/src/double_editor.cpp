#include <dak/ui_qt/double_editor.h>

#include <dak/utility/number.h>

#include <cmath>

namespace dak
{
   namespace ui_qt
   {
      static constexpr double double_to_int_factor = 100.;

      double_editor::double_editor(QWidget* parent, const wchar_t* label_text)
      : double_editor(parent, label_text, NAN, nullptr)
      {
      }

      double_editor::double_editor(QWidget* parent, const wchar_t* label_text, double value)
      : double_editor(parent, label_text, value, nullptr)
      {
      }

      double_editor::double_editor(QWidget* parent, const wchar_t* label_text, double value, value_changed_callback changed_callback)
      : QWidget(parent), current_value(NAN), value_changed(changed_callback)
      {
         build_ui(label_text);
         set_value(value);
      }

      void double_editor::set_limits(double min, double max, double step)
      {
         slider->setRange(int(std::round(min * double_to_int_factor)), int(std::round(max * double_to_int_factor)));
         text_editor->setRange(min, max);
         text_editor->setSingleStep(step);
      }

      void double_editor::set_value(double new_value, bool call_callback)
      {
         if (utility::near(current_value, new_value))
            return;

         auto prev_callback = value_changed;
         if (!call_callback)
            value_changed = nullptr;

         current_value = new_value;

         const int desired_slider_value = std::round(new_value * double_to_int_factor);
         if (slider->value() != desired_slider_value)
            slider->setValue(desired_slider_value);

         if (!utility::near(text_editor->value(), new_value))
            text_editor->setValue(new_value);

         if (value_changed)
            value_changed(new_value, false);

         if (!call_callback)
            value_changed = prev_callback;
      }

      void double_editor::set_value_from_spin_box(double new_value)
      {
         if (utility::near(current_value, new_value))
            return;

         current_value = new_value;

         const int desired_slider_value = std::round(new_value * double_to_int_factor);
         if (slider->value() != desired_slider_value)
            slider->setValue(desired_slider_value);

         if (value_changed)
            value_changed(new_value, false);
      }

      void double_editor::set_value_from_slider(int new_value, bool force_update)
      {
         const double real_new_value = new_value / double_to_int_factor;

         if (!force_update && utility::near(current_value, real_new_value))
            return;

         current_value = real_new_value;

         if (!utility::near(text_editor->value(), real_new_value))
            text_editor->setValue(real_new_value);

         if (value_changed)
            value_changed(real_new_value, slider->isSliderDown());
      }

      void double_editor::set_value_from_slider(int new_value)
      {
         set_value_from_slider(new_value, false);
      }

      void double_editor::slider_pressed()
      {
         start_value = slider->value();
      }

      void double_editor::slider_released()
      {
         const int new_value = slider->value();
         set_value_from_slider(new_value, start_value != new_value);
      }

      void double_editor::build_ui(const wchar_t* label_text)
      {
         label = new QLabel(QString::fromWCharArray(label_text), this);
         slider = new QSlider(Qt::Orientation::Horizontal, this);
         text_editor = new QDoubleSpinBox(this);

         layout = new QHBoxLayout(this);
         layout->setContentsMargins(0, 0, 0, 0);
         layout->addWidget(label);
         layout->addWidget(slider);
         layout->addWidget(text_editor);

         slider->setRange(-1000, 1000);
         text_editor->setRange(-10., 10.);
         text_editor->setSingleStep(0.1);
         text_editor->setDecimals(3);

         slider->connect(slider, &QSlider::valueChanged, [self=this](int new_value) { self->set_value_from_slider(new_value); });
         slider->connect(slider, &QSlider::sliderPressed, this, &double_editor::slider_released);
         slider->connect(slider, &QSlider::sliderReleased, this, &double_editor::slider_released);

         void (QDoubleSpinBox::*vc)(double) = &QDoubleSpinBox::valueChanged;
         text_editor->connect(text_editor, vc, this, &double_editor::set_value_from_spin_box);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
