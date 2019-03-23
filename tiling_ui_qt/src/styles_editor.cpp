#include <dak/tiling_ui_qt/styles_editor.h>

#include <dak/ui_qt/convert.h>
#include <dak/ui_qt/int_editor.h>
#include <dak/ui_qt/double_editor.h>

#include <dak/tiling_style/thick.h>
#include <dak/tiling_style/emboss.h>
#include <dak/tiling_style/filled.h>

#include <dak/geometry/utility.h>

#include <QtGui/qicon.h>
#include <QtGui/qpixmap.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qcolordialog.h>
#include <QtWidgets/qpushbutton.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling_style::colored;
      using tiling_style::thick;
      using tiling_style::emboss;
      using tiling_style::filled;

      using geometry::L;
      typedef std::vector<std::shared_ptr<style>> styles;
      typedef std::function<void(const styles& )> styles_changed_callback;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a style.

      class styles_editor_ui
      {
      public:
         styles_editor_ui(styles_editor& parent, const styles& ed)
         : editor(parent)
         {
            build_ui(parent);
            set_edited(ed);
         }

         const styles& get_edited() const
         {
            return edited;
         }

         void set_edited(const styles& ed)
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

         std::vector<dak::ui::color*> get_style_colors()
         {
            std::vector<dak::ui::color*> selected;
            for (auto style : get_styles<colored>())
               selected.emplace_back(&style->color);
            return selected;
         };

         std::vector<dak::ui::color*> get_style_outline_colors()
         {
            std::vector<dak::ui::color*> selected;
            for (auto style : get_styles<thick>())
               selected.emplace_back(&style->outline_color);
            return selected;
         };

         void build_ui(styles_editor& parent)
         {
            QVBoxLayout* layout = new QVBoxLayout(&parent);
               layout->setContentsMargins(0, 0, 0, 0);
               QWidget* style_buttons_panel = new QWidget(&parent);
               QHBoxLayout* style_buttons_layout = new QHBoxLayout(style_buttons_panel);
                  style_buttons_layout->setContentsMargins(0, 0, 0, 0);
                  color_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Color")), &parent);
                  style_buttons_layout->addWidget(color_button.get());
                  outline_color_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Outline")), &parent);
                  style_buttons_layout->addWidget(outline_color_button.get());
               layout->addWidget(style_buttons_panel);

               QWidget* fill_panel = new QWidget(&parent);
               QHBoxLayout* fill_layout = new QHBoxLayout(fill_panel);
                  fill_layout->setContentsMargins(0, 0, 0, 0);
                  fill_inside_check = std::make_unique<QCheckBox>(QString::fromWCharArray(L::t(L"Fill Inside")), &parent);
                  fill_layout->addWidget(fill_inside_check.get());
                  fill_outside_check = std::make_unique<QCheckBox>(QString::fromWCharArray(L::t(L"Fill Outside")), &parent);
                  fill_layout->addWidget(fill_outside_check.get());
               layout->addWidget(fill_panel);

               QWidget* style_outline_panel = new QWidget(&parent);
               QVBoxLayout* style_outline_layout = new QVBoxLayout(style_outline_panel);
                  style_outline_layout->setContentsMargins(0, 0, 0, 0);
                  width_editor = std::make_unique<dak::ui_qt::double_editor>(&parent, L::t(L"Width"));
                  width_editor->set_limits(0.01, 40, 0.01);
                  style_outline_layout->addWidget(width_editor.get());
                  outline_width_editor = std::make_unique<dak::ui_qt::double_editor>(&parent, L::t(L"Outline Width"));
                  outline_width_editor->set_limits(0, 20, 0.01);
                  style_outline_layout->addWidget(outline_width_editor.get());
                  angle_editor = std::make_unique<ui_qt::double_editor>(&parent, L::t(L"Angle"));
                  style_outline_layout->addWidget(angle_editor.get());
               layout->addWidget(style_outline_panel);

            color_button->setEnabled(false);
            outline_color_button->setEnabled(false);
            width_editor->setEnabled(false);
            outline_width_editor->setEnabled(false);
            fill_inside_check->setEnabled(false);
            fill_outside_check->setEnabled(false);
            angle_editor->setEnabled(false);

            color_button->connect(color_button.get(), &QPushButton::clicked, [&]() { update_color(); });
            outline_color_button->connect(outline_color_button.get(), &QPushButton::clicked, [&]() { update_outline_color(); });
            width_editor->value_changed_callback = std::bind(&styles_editor_ui::update_width, this, std::placeholders::_1);
            outline_width_editor->value_changed_callback = std::bind(&styles_editor_ui::update_outline_width, this, std::placeholders::_1);
            fill_inside_check->connect(fill_inside_check.get(), &QCheckBox::stateChanged, [&](int new_state) { update_fill_inside(new_state); });
            fill_outside_check->connect(fill_outside_check.get(), &QCheckBox::stateChanged, [&](int new_state) { update_fill_outside(new_state); });
            angle_editor->value_changed_callback = std::bind(&styles_editor_ui::update_angle, this, std::placeholders::_1);
         }

         void fill_ui()
         {
            if (get_styles<thick>().size() > 0)
            {
               width_editor->setEnabled(true);
               outline_width_editor->setEnabled(true);
               outline_color_button->setEnabled(true);
               fill_ui_outline_color();
               for (auto thick : get_styles<thick>())
               {
                  width_editor->set_value(thick->width);
                  outline_width_editor->set_value(thick->outline_width);
               }
            }
            else
            {
               width_editor->setEnabled(false);
               outline_width_editor->setEnabled(false);
               outline_color_button->setEnabled(false);
            }

            if (get_styles<colored>().size() > 0)
            {
               color_button->setEnabled(true);
               fill_ui_color();
            }
            else
            {
               color_button->setEnabled(false);
            }

            if (get_styles<emboss>().size() > 0)
            {
               angle_editor->setEnabled(true);
               for (auto emb : get_styles<emboss>())
               {
                  angle_editor->set_value(emb->angle);
                  break;
               }
            }
            else
            {
               angle_editor->setEnabled(false);
            }

            if (get_styles<filled>().size() > 0)
            {
               fill_inside_check->setEnabled(true);
               fill_outside_check->setEnabled(true);
               for (auto fi : get_styles<filled>())
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
            if (get_styles<colored>().size() > 0)
            {
               for (auto colored : get_styles<colored>())
               {
                  QPixmap color(16, 16);
                  color.fill(ui_qt::convert(colored->color));
                  color_button->setIcon(QIcon(color));
                  break;
               }
            }
         }

         void fill_ui_outline_color()
         {
            for (auto thick : get_styles<thick>())
            {
               QPixmap color(16, 16);
               color.fill(ui_qt::convert(thick->outline_color));
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

            QColor qc = QColorDialog::getColor(dak::ui_qt::convert(*colors[0]), color_button.get(), QString::fromWCharArray(L::t(L"Choose Layer Color")), QColorDialog::ColorDialogOption::ShowAlphaChannel);
            if (!qc.isValid())
               return;

            for (auto& c : colors)
               *c = dak::ui_qt::convert(qc);

            fill_ui_color();

            update();
         }

         void update_outline_color()
         {
            if (disable_feedback)
               return;

            auto colors = get_style_outline_colors();
            if (colors.size() <= 0)
               return;

            QColor qc = QColorDialog::getColor(dak::ui_qt::convert(*colors[0]), outline_color_button.get(), QString::fromWCharArray(L::t(L"Choose Layer Outline Color")), QColorDialog::ColorDialogOption::ShowAlphaChannel);
            if (!qc.isValid())
               return;

            for (auto& c : colors)
               *c = dak::ui_qt::convert(qc);

            fill_ui_outline_color();

            update();
         }

         void update_width(double new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<thick>())
               style->width = new_value;
            update();
         }

         void update_outline_width(double new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<thick>())
               style->outline_width = new_value;
            update();
         }

         void update_angle(double new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<emboss>())
               style->angle = new_value;
            update();
         }

         void update_fill_inside(int new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<filled>())
               style->draw_inside = (new_value != 0);
            update();
         }

         void update_fill_outside(int new_value)
         {
            if (disable_feedback)
               return;

            for (auto style : get_styles<filled>())
               style->draw_outside = (new_value != 0);
            update();
         }

         void update()
         {
            if (edited.size() <= 0)
               return;

            // Note: used to avoid re-calculating the style when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.styles_changed)
               editor.styles_changed(edited);
         }

         styles_editor& editor;
         styles edited;

         std::unique_ptr<QPushButton> color_button;
         std::unique_ptr<QPushButton> outline_color_button;
         std::unique_ptr<ui_qt::double_editor> width_editor;
         std::unique_ptr<ui_qt::double_editor> outline_width_editor;
         std::unique_ptr<ui_qt::double_editor> angle_editor;
         std::unique_ptr<QCheckBox> fill_inside_check;
         std::unique_ptr<QCheckBox> fill_outside_check;

         bool disable_feedback = false;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a style.

      styles_editor::styles_editor(QWidget* parent)
      : styles_editor(parent, {}, nullptr)
      {
      }

      styles_editor::styles_editor(QWidget* parent, styles_changed_callback fc)
      : styles_editor(parent, {}, fc)
      {
      }

      styles_editor::styles_editor(QWidget* parent, const styles& edited, styles_changed_callback fc)
      : QWidget(parent), ui(std::make_unique<styles_editor_ui>(*this, edited)), styles_changed(fc)
      {
      }

      void styles_editor::set_edited(const styles& edited)
      {
         ui->set_edited(edited);
      }

      const styles& styles_editor::get_edited() const
      {
         static const styles empty;
         if (!ui)
            return empty;

         return ui->get_edited();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
