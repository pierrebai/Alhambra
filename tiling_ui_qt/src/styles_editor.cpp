#include <dak/tiling_ui_qt/styles_editor.h>

#include <dak/ui/qt/convert.h>
#include <dak/ui/qt/int_editor.h>
#include <dak/ui/qt/double_editor.h>
#include <dak/ui/qt/color_editor.h>

#include <dak/tiling_style/thick.h>
#include <dak/tiling_style/emboss.h>
#include <dak/tiling_style/filled.h>
#include <dak/tiling_style/interlace.h>

#include <dak/utility/text.h>

#include <QtGui/qicon.h>
#include <QtGui/qpixmap.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qcolordialog.h>
#include <QtWidgets/qpushbutton.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling_style::colored_t;
      using tiling_style::thick_t;
      using tiling_style::emboss_t;
      using tiling_style::filled_t;
      using tiling_style::interlace_t;

      using utility::L;
      typedef std::vector<std::shared_ptr<style_t>> styles_t;
      typedef std::function<void(const styles_t& )> styles_changed_callback;

      namespace
      {
         struct
         {
            ui::stroke_t::join_style_t style;
            const wchar_t* name;
         }
         const join_style_names[] =
         {
            { ui::stroke_t::join_style_t::bevel, L"Beveled Corners" },
            { ui::stroke_t::join_style_t::miter, L"Mitered Corners" },
            { ui::stroke_t::join_style_t::round, L"Rounded Corners" },
         };

         const wchar_t* get_join_style_name(const ui::stroke_t::join_style_t a_style)
         {
            for (const auto& style : join_style_names)
               if (style.style == a_style)
                  return L::t(style.name);
            return L::t(L"Unknown");
         }

         ui::stroke_t::join_style_t get_join_style_from_name(const std::wstring& a_style)
         {
            for (const auto& style : join_style_names)
               if (std::wstring(L::t(style.name)) == a_style)
                  return style.style;
            return ui::stroke_t::join_style_t::round;
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a style.

      class styles_editor_ui_t
      {
      public:
         styles_editor_ui_t(styles_editor_t& parent, const styles_t& ed)
         : my_styles_editor(parent)
         {
            build_ui(parent);
            set_edited(ed);
         }

         const styles_t& get_edited() const
         {
            return my_edited_styles;
         }

         void set_edited(const styles_t& ed)
         {
            if (ed == my_edited_styles)
               return;

            my_disable_feedback = true;
            my_edited_styles = ed;
            fill_ui();
            my_disable_feedback = false;
         }

      private:
         template <class some_style>
         std::vector<std::shared_ptr<some_style>> get_styles()
         {
            std::vector<std::shared_ptr<some_style>> selected;
            for (auto style : my_edited_styles)
               if (auto st = std::dynamic_pointer_cast<some_style>(style))
                  selected.emplace_back(st);
            return selected;
         }

         std::vector<dak::ui::color_t*> get_style_colors()
         {
            std::vector<dak::ui::color_t*> selected;
            for (auto style : get_styles<colored_t>())
               selected.emplace_back(&style->color);
            return selected;
         };

         std::vector<dak::ui::color_t*> get_style_outline_colors()
         {
            std::vector<dak::ui::color_t*> selected;
            for (auto style : get_styles<thick_t>())
               selected.emplace_back(&style->outline_color);
            return selected;
         };

         void build_ui(styles_editor_t& parent)
         {
            QVBoxLayout* layout = new QVBoxLayout(&parent);
               layout->setContentsMargins(0, 0, 0, 0);
               QWidget* style_buttons_panel = new QWidget(&parent);
               QHBoxLayout* style_buttons_layout = new QHBoxLayout(style_buttons_panel);
                  style_buttons_layout->setContentsMargins(0, 0, 0, 0);
                  my_color_button = std::make_unique<ui::qt::color_editor_t>(style_buttons_panel, L::t(L"Color"));
                  style_buttons_layout->addWidget(my_color_button.get());
                  my_outline_color_button = std::make_unique<ui::qt::color_editor_t>(style_buttons_panel, L::t(L"Outline"));
                  style_buttons_layout->addWidget(my_outline_color_button.get());
                  my_join_combobox = std::make_unique<QComboBox>(style_buttons_panel);
                  for (const auto& style : join_style_names)
                     my_join_combobox->addItem(QString::fromWCharArray(L::t(style.name)));
                  style_buttons_layout->addWidget(my_join_combobox.get());
               layout->addWidget(style_buttons_panel);


               QWidget* fill_panel = new QWidget(&parent);
               QHBoxLayout* fill_layout = new QHBoxLayout(fill_panel);
                  fill_layout->setContentsMargins(0, 0, 0, 0);
                  my_fill_inside_checkbox = std::make_unique<QCheckBox>(QString::fromWCharArray(L::t(L"Fill Inside")), fill_panel);
                  fill_layout->addWidget(my_fill_inside_checkbox.get());
                  my_fill_outside_checkbox = std::make_unique<QCheckBox>(QString::fromWCharArray(L::t(L"Fill Outside")), fill_panel);
                  fill_layout->addWidget(my_fill_outside_checkbox.get());
               layout->addWidget(fill_panel);

               QWidget* style_panel = new QWidget(&parent);
               QVBoxLayout* style_layout = new QVBoxLayout(style_panel);
                  style_layout->setContentsMargins(0, 0, 0, 0);
                  my_width_editor = std::make_unique<dak::ui::qt::double_editor_t>(style_panel, L::t(L"Width"));
                  my_width_editor->set_limits(0.001, 40, 0.01);
                  style_layout->addWidget(my_width_editor.get());
                  my_outline_width_editor = std::make_unique<dak::ui::qt::double_editor_t>(style_panel, L::t(L"Outline Width"));
                  my_outline_width_editor->set_limits(0, 20, 0.01);
                  style_layout->addWidget(my_outline_width_editor.get());
                  my_gap_width_editor = std::make_unique<dak::ui::qt::double_editor_t>(style_panel, L::t(L"Gap Width"));
                  my_gap_width_editor->set_limits(0, 20, 0.01);
                  style_layout->addWidget(my_gap_width_editor.get());
                  my_angle_editor = std::make_unique<ui::qt::double_editor_t>(style_panel, L::t(L"Angle"));
                  style_layout->addWidget(my_angle_editor.get());
               layout->addWidget(style_panel);

            my_color_button->setEnabled(false);
            my_outline_color_button->setEnabled(false);
            my_width_editor->setEnabled(false);
            my_outline_width_editor->setEnabled(false);
            my_gap_width_editor->setEnabled(false);
            my_fill_inside_checkbox->setEnabled(false);
            my_fill_outside_checkbox->setEnabled(false);
            my_angle_editor->setEnabled(false);
            my_join_combobox->setEnabled(false);

            my_color_button->on_color_changed = [&](ui::color_t a_color) { update_color(a_color); };
            my_outline_color_button->on_color_changed = [&](ui::color_t a_color) { update_outline_color(a_color); };
            my_width_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_width(new_value, interacting); };
            my_outline_width_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_outline_width(new_value, interacting); };
            my_gap_width_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_gap_width(new_value, interacting); };
            my_fill_inside_checkbox->connect(my_fill_inside_checkbox.get(), &QCheckBox::stateChanged, [&](int new_state) { update_fill_inside(new_state); });
            my_fill_outside_checkbox->connect(my_fill_outside_checkbox.get(), &QCheckBox::stateChanged, [&](int new_state) { update_fill_outside(new_state); });
            my_angle_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_angle(new_value, interacting); };
            my_join_combobox->connect(my_join_combobox.get(), &QComboBox::currentTextChanged, [&](const QString& text) { update_join(text); });
         }

         void fill_ui()
         {
            if (get_styles<thick_t>().size() > 0)
            {
               my_width_editor->setEnabled(true);
               my_outline_width_editor->setEnabled(true);
               my_outline_color_button->setEnabled(true);
               my_join_combobox->setEnabled(true);
               fill_ui_outline_color();
               double max_width = 0.1;
               double max_outline_width = 0.1;
               for (auto thick : get_styles<thick_t>())
               {
                  max_width = std::max(max_width, thick->width);
                  max_outline_width = std::max(max_outline_width, thick->outline_width);

                  my_width_editor->set_value(thick->width);
                  my_outline_width_editor->set_value(thick->outline_width);
                  my_join_combobox->setCurrentText(QString::fromWCharArray(get_join_style_name(thick->join)));
               }
               my_width_editor->set_limits(0.001, std::max(1., max_width * 10.), 0.01);
               my_outline_width_editor->set_limits(0., std::max(1., max_outline_width * 10.), 0.01);
            }
            else
            {
               my_width_editor->setEnabled(false);
               my_outline_width_editor->setEnabled(false);
               my_outline_color_button->setEnabled(false);
               my_join_combobox->setEnabled(false);
            }

            if (get_styles<interlace_t>().size() > 0)
            {
               my_gap_width_editor->setEnabled(true);
               double max_gap_width = 0.1;
               for (auto inter : get_styles<interlace_t>())
               {
                  max_gap_width = std::max(max_gap_width, inter->gap_width);
                  my_gap_width_editor->set_value(inter->gap_width);
               }
               my_gap_width_editor->set_limits(0., std::max(1., max_gap_width * 10.), 0.01);
            }
            else
            {
               my_gap_width_editor->setEnabled(false);
            }

            if (get_styles<colored_t>().size() > 0)
            {
               my_color_button->setEnabled(true);
               fill_ui_color();
            }
            else
            {
               my_color_button->setEnabled(false);
            }

            if (get_styles<emboss_t>().size() > 0)
            {
               my_angle_editor->setEnabled(true);
               for (auto emb : get_styles<emboss_t>())
               {
                  my_angle_editor->set_value(emb->angle);
                  break;
               }
            }
            else
            {
               my_angle_editor->setEnabled(false);
            }

            if (get_styles<filled_t>().size() > 0)
            {
               my_fill_inside_checkbox->setEnabled(true);
               my_fill_outside_checkbox->setEnabled(true);
               for (auto fi : get_styles<filled_t>())
               {
                  my_fill_inside_checkbox->setChecked(fi->draw_inside);
                  my_fill_outside_checkbox->setChecked(fi->draw_outside);
                  break;
               }
            }
            else
            {
               my_fill_inside_checkbox->setEnabled(false);
               my_fill_outside_checkbox->setEnabled(false);
            }
         }

         void fill_ui_color()
         {
            if (get_styles<colored_t>().size() > 0)
            {
               for (auto colored : get_styles<colored_t>())
               {
                  my_color_button->set_color(colored->color);
                  break;
               }
            }
         }

         void fill_ui_outline_color()
         {
            for (auto thick : get_styles<thick_t>())
            {
               my_outline_color_button->set_color(thick->outline_color);
               break;
            }
         }

         void update_color(ui::color_t a_color)
         {
            if (my_disable_feedback)
               return;

            auto colors = get_style_colors();
            if (colors.size() <= 0)
               return;

            for (auto& c : colors)
               *c = a_color;

            fill_ui_color();

            update(false);
         }

         void update_outline_color(ui::color_t a_color)
         {
            if (my_disable_feedback)
               return;

            auto colors = get_style_outline_colors();
            if (colors.size() <= 0)
               return;

            for (auto& c : colors)
               *c = a_color;

            fill_ui_outline_color();

            update(false);
         }

         void update_width(double new_value, bool interacting)
         {
            if (my_disable_feedback)
               return;

            for (auto style : get_styles<thick_t>())
               style->width = new_value;
            update(interacting);
         }

         void update_outline_width(double new_value, bool interacting)
         {
            if (my_disable_feedback)
               return;

            for (auto style : get_styles<thick_t>())
               style->outline_width = new_value;
            update(interacting);
         }

         void update_gap_width(double new_value, bool interacting)
         {
            if (my_disable_feedback)
               return;

            for (auto style : get_styles<interlace_t>())
               style->gap_width = new_value;
            update(interacting);
         }

         void update_angle(double new_value, bool interacting)
         {
            if (my_disable_feedback)
               return;

            for (auto style : get_styles<emboss_t>())
               style->angle = new_value;
            update(interacting);
         }

         void update_fill_inside(int new_value)
         {
            if (my_disable_feedback)
               return;

            for (auto style : get_styles<filled_t>())
               style->draw_inside = (new_value != 0);
            update(false);
         }

         void update_fill_outside(int new_value)
         {
            if (my_disable_feedback)
               return;

            for (auto style : get_styles<filled_t>())
               style->draw_outside = (new_value != 0);
            update(false);
         }

         void update_join(const QString& new_value)
         {
            if (my_disable_feedback)
               return;

            for (auto style : get_styles<thick_t>())
               style->join = get_join_style_from_name(new_value.toStdWString());
            update(false);
         }

         void update(bool interacting)
         {
            if (my_edited_styles.size() <= 0)
               return;

            // Note: used to avoid re-calculating the style when just setting its value in the UI.
            if (my_disable_feedback)
               return;

            if (my_styles_editor.styles_changed)
               my_styles_editor.styles_changed(my_edited_styles, interacting);
         }

         styles_editor_t& my_styles_editor;
         styles_t my_edited_styles;

         std::unique_ptr<ui::qt::color_editor_t> my_color_button;
         std::unique_ptr<ui::qt::color_editor_t> my_outline_color_button;
         std::unique_ptr<ui::qt::double_editor_t> my_width_editor;
         std::unique_ptr<ui::qt::double_editor_t> my_outline_width_editor;
         std::unique_ptr<ui::qt::double_editor_t> my_gap_width_editor;
         std::unique_ptr<ui::qt::double_editor_t> my_angle_editor;
         std::unique_ptr<QCheckBox> my_fill_inside_checkbox;
         std::unique_ptr<QCheckBox> my_fill_outside_checkbox;
         std::unique_ptr<QComboBox> my_join_combobox;

         bool my_disable_feedback = false;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a style.

      styles_editor_t::styles_editor_t(QWidget* parent)
      : styles_editor_t(parent, {}, nullptr)
      {
      }

      styles_editor_t::styles_editor_t(QWidget* parent, styles_changed_callback fc)
      : styles_editor_t(parent, {}, fc)
      {
      }

      styles_editor_t::styles_editor_t(QWidget* parent, const styles_t& edited_styles, styles_changed_callback fc)
      : QWidget(parent), my_ui(std::make_unique<styles_editor_ui_t>(*this, edited_styles)), styles_changed(fc)
      {
      }

      void styles_editor_t::set_edited(const styles_t& edited_styles)
      {
         my_ui->set_edited(edited_styles);
      }

      const styles_t& styles_editor_t::get_edited() const
      {
         static const styles_t empty;
         if (!my_ui)
            return empty;

         return my_ui->get_edited();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
