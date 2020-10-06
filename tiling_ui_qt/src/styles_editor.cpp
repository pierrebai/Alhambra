#include <dak/tiling_ui_qt/styles_editor.h>

#include <dak/ui/qt/convert.h>
#include <dak/ui/qt/int_editor.h>
#include <dak/ui/qt/double_editor.h>

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
         const join_styles[] =
         {
            { ui::stroke_t::join_style_t::bevel, L"Beveled Corners" },
            { ui::stroke_t::join_style_t::miter, L"Mitered Corners" },
            { ui::stroke_t::join_style_t::round, L"Rounded Corners" },
         };

         const wchar_t* get_join_style_name(const ui::stroke_t::join_style_t a_style)
         {
            for (const auto& style : join_styles)
               if (style.style == a_style)
                  return L::t(style.name);
            return L::t(L"Unknown");
         }

         ui::stroke_t::join_style_t get_join_style_from_name(const std::wstring& a_style)
         {
            for (const auto& style : join_styles)
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
         : editor(parent)
         {
            build_ui(parent);
            set_edited(ed);
         }

         const styles_t& get_edited() const
         {
            return edited;
         }

         void set_edited(const styles_t& ed)
         {
            if (ed == edited)
               return;

            disable_feedback = true;
            edited = ed;
            fill_ui();
            disable_feedback = false;
         }

      private:
         template <class some_style>
         std::vector<std::shared_ptr<some_style>> get_styles()
         {
            std::vector<std::shared_ptr<some_style>> selected;
            for (auto style : edited)
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
                  color_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Color")), style_buttons_panel);
                  style_buttons_layout->addWidget(color_button.get());
                  outline_color_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Outline")), style_buttons_panel);
                  style_buttons_layout->addWidget(outline_color_button.get());
                  join_combo = std::make_unique<QComboBox>(style_buttons_panel);
                  for (const auto& style : join_styles)
                     join_combo->addItem(QString::fromWCharArray(L::t(style.name)));
                  style_buttons_layout->addWidget(join_combo.get());
               layout->addWidget(style_buttons_panel);


               QWidget* fill_panel = new QWidget(&parent);
               QHBoxLayout* fill_layout = new QHBoxLayout(fill_panel);
                  fill_layout->setContentsMargins(0, 0, 0, 0);
                  fill_inside_check = std::make_unique<QCheckBox>(QString::fromWCharArray(L::t(L"Fill Inside")), fill_panel);
                  fill_layout->addWidget(fill_inside_check.get());
                  fill_outside_check = std::make_unique<QCheckBox>(QString::fromWCharArray(L::t(L"Fill Outside")), fill_panel);
                  fill_layout->addWidget(fill_outside_check.get());
               layout->addWidget(fill_panel);

               QWidget* style_panel = new QWidget(&parent);
               QVBoxLayout* style_layout = new QVBoxLayout(style_panel);
                  style_layout->setContentsMargins(0, 0, 0, 0);
                  width_editor = std::make_unique<dak::ui::qt::double_editor_t>(style_panel, L::t(L"Width"));
                  width_editor->set_limits(0.001, 40, 0.01);
                  style_layout->addWidget(width_editor.get());
                  outline_width_editor = std::make_unique<dak::ui::qt::double_editor_t>(style_panel, L::t(L"Outline Width"));
                  outline_width_editor->set_limits(0, 20, 0.01);
                  style_layout->addWidget(outline_width_editor.get());
                  gap_width_editor = std::make_unique<dak::ui::qt::double_editor_t>(style_panel, L::t(L"Gap Width"));
                  gap_width_editor->set_limits(0, 20, 0.01);
                  style_layout->addWidget(gap_width_editor.get());
                  angle_editor = std::make_unique<ui::qt::double_editor_t>(style_panel, L::t(L"Angle"));
                  style_layout->addWidget(angle_editor.get());
               layout->addWidget(style_panel);

            color_button->setEnabled(false);
            outline_color_button->setEnabled(false);
            width_editor->setEnabled(false);
            outline_width_editor->setEnabled(false);
            gap_width_editor->setEnabled(false);
            fill_inside_check->setEnabled(false);
            fill_outside_check->setEnabled(false);
            angle_editor->setEnabled(false);
            join_combo->setEnabled(false);

            color_button->connect(color_button.get(), &QPushButton::clicked, [&]() { update_color(); });
            outline_color_button->connect(outline_color_button.get(), &QPushButton::clicked, [&]() { update_outline_color(); });
            width_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_width(new_value, interacting); };
            outline_width_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_outline_width(new_value, interacting); };
            gap_width_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_gap_width(new_value, interacting); };
            fill_inside_check->connect(fill_inside_check.get(), &QCheckBox::stateChanged, [&](int new_state) { update_fill_inside(new_state); });
            fill_outside_check->connect(fill_outside_check.get(), &QCheckBox::stateChanged, [&](int new_state) { update_fill_outside(new_state); });
            angle_editor->value_changed = [self=this](double new_value, bool interacting) { self->update_angle(new_value, interacting); };
            join_combo->connect(join_combo.get(), &QComboBox::currentTextChanged, [&](const QString& text) { update_join(text); });
         }

         void fill_ui()
         {
            if (get_styles<thick_t>().size() > 0)
            {
               width_editor->setEnabled(true);
               outline_width_editor->setEnabled(true);
               outline_color_button->setEnabled(true);
               join_combo->setEnabled(true);
               fill_ui_outline_color();
               double max_width = 0.1;
               double max_outline_width = 0.1;
               for (auto thick : get_styles<thick_t>())
               {
                  max_width = std::max(max_width, thick->width);
                  max_outline_width = std::max(max_outline_width, thick->outline_width);

                  width_editor->set_value(thick->width);
                  outline_width_editor->set_value(thick->outline_width);
                  join_combo->setCurrentText(QString::fromWCharArray(get_join_style_name(thick->join)));
               }
               width_editor->set_limits(0.001, std::max(1., max_width * 10.), 0.01);
               outline_width_editor->set_limits(0., std::max(1., max_outline_width * 10.), 0.01);
            }
            else
            {
               width_editor->setEnabled(false);
               outline_width_editor->setEnabled(false);
               outline_color_button->setEnabled(false);
               join_combo->setEnabled(false);
            }

            if (get_styles<interlace_t>().size() > 0)
            {
               gap_width_editor->setEnabled(true);
               double max_gap_width = 0.1;
               for (auto inter : get_styles<interlace_t>())
               {
                  max_gap_width = std::max(max_gap_width, inter->gap_width);
                  gap_width_editor->set_value(inter->gap_width);
               }
               gap_width_editor->set_limits(0., std::max(1., max_gap_width * 10.), 0.01);
            }
            else
            {
               gap_width_editor->setEnabled(false);
            }

            if (get_styles<colored_t>().size() > 0)
            {
               color_button->setEnabled(true);
               fill_ui_color();
            }
            else
            {
               color_button->setEnabled(false);
            }

            if (get_styles<emboss_t>().size() > 0)
            {
               angle_editor->setEnabled(true);
               for (auto emb : get_styles<emboss_t>())
               {
                  angle_editor->set_value(emb->angle);
                  break;
               }
            }
            else
            {
               angle_editor->setEnabled(false);
            }

            if (get_styles<filled_t>().size() > 0)
            {
               fill_inside_check->setEnabled(true);
               fill_outside_check->setEnabled(true);
               for (auto fi : get_styles<filled_t>())
               {
                  fill_inside_check->setChecked(fi->draw_inside);
                  fill_outside_check->setChecked(fi->draw_outside);
                  break;
               }
            }
            else
            {
               fill_inside_check->setEnabled(false);
               fill_outside_check->setEnabled(false);
            }
         }

         void fill_ui_color()
         {
            if (get_styles<colored_t>().size() > 0)
            {
               for (auto colored : get_styles<colored_t>())
               {
                  QPixmap color(16, 16);
                  color.fill(ui::qt::convert(colored->color));
                  color_button->setIcon(QIcon(color));
                  break;
               }
            }
         }

         void fill_ui_outline_color()
         {
            for (auto thick : get_styles<thick_t>())
            {
               QPixmap color(16, 16);
               color.fill(ui::qt::convert(thick->outline_color));
               outline_color_button->setIcon(QIcon(color));
               break;
            }
         }

         void update_color()
         {
            if (disable_feedback)
               return;

            auto colors = get_style_colors();
            if (colors.size() <= 0)
               return;

            QColor qc = QColorDialog::getColor(dak::ui::qt::convert(*colors[0]), color_button.get(), QString::fromWCharArray(L::t(L"Choose Layer Color")), QColorDialog::ColorDialogOption::ShowAlphaChannel);
            if (!qc.isValid())
               return;

            for (auto& c : colors)
               *c = dak::ui::qt::convert(qc);

            fill_ui_color();

            update(false);
         }

         void update_outline_color()
         {
            if (disable_feedback)
               return;

            auto colors = get_style_outline_colors();
            if (colors.size() <= 0)
               return;

            QColor qc = QColorDialog::getColor(dak::ui::qt::convert(*colors[0]), outline_color_button.get(), QString::fromWCharArray(L::t(L"Choose Layer Outline Color")), QColorDialog::ColorDialogOption::ShowAlphaChannel);
            if (!qc.isValid())
               return;

            for (auto& c : colors)
               *c = dak::ui::qt::convert(qc);

            fill_ui_outline_color();

            update(false);
         }

         void update_width(double new_value, bool interacting)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<thick_t>())
               style->width = new_value;
            update(interacting);
         }

         void update_outline_width(double new_value, bool interacting)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<thick_t>())
               style->outline_width = new_value;
            update(interacting);
         }

         void update_gap_width(double new_value, bool interacting)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<interlace_t>())
               style->gap_width = new_value;
            update(interacting);
         }

         void update_angle(double new_value, bool interacting)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<emboss_t>())
               style->angle = new_value;
            update(interacting);
         }

         void update_fill_inside(int new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<filled_t>())
               style->draw_inside = (new_value != 0);
            update(false);
         }

         void update_fill_outside(int new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<filled_t>())
               style->draw_outside = (new_value != 0);
            update(false);
         }

         void update_join(const QString& new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<thick_t>())
               style->join = get_join_style_from_name(new_value.toStdWString());
            update(false);
         }

         void update(bool interacting)
         {
            if (edited.size() <= 0)
               return;

            // Note: used to avoid re-calculating the style when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.styles_changed)
               editor.styles_changed(edited, interacting);
         }

         styles_editor_t& editor;
         styles_t edited;

         std::unique_ptr<QPushButton> color_button;
         std::unique_ptr<QPushButton> outline_color_button;
         std::unique_ptr<ui::qt::double_editor_t> width_editor;
         std::unique_ptr<ui::qt::double_editor_t> outline_width_editor;
         std::unique_ptr<ui::qt::double_editor_t> gap_width_editor;
         std::unique_ptr<ui::qt::double_editor_t> angle_editor;
         std::unique_ptr<QCheckBox> fill_inside_check;
         std::unique_ptr<QCheckBox> fill_outside_check;
         std::unique_ptr<QComboBox> join_combo;

         bool disable_feedback = false;
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

      styles_editor_t::styles_editor_t(QWidget* parent, const styles_t& edited, styles_changed_callback fc)
      : QWidget(parent), ui(std::make_unique<styles_editor_ui_t>(*this, edited)), styles_changed(fc)
      {
      }

      void styles_editor_t::set_edited(const styles_t& edited)
      {
         ui->set_edited(edited);
      }

      const styles_t& styles_editor_t::get_edited() const
      {
         static const styles_t empty;
         if (!ui)
            return empty;

         return ui->get_edited();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
