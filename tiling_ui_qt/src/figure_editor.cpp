#include <dak/tiling_ui_qt/figure_editor.h>

#include <dak/tiling/star.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/irregular_figure.h>
#include <dak/tiling/extended_figure.h>

#include <dak/ui/qt/int_editor.h>
#include <dak/ui/qt/double_editor.h>

#include <dak/utility/text.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcombobox.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling::star_t;
      using tiling::rosette_t;
      using tiling::irregular_figure_t;
      using tiling::extended_figure_t;
      using utility::L;
      typedef std::function<void(std::shared_ptr<figure_t>)> figure_changed_callback;

      namespace
      {
         star_t* get_star(std::shared_ptr<figure_t>& my_edited_figure)
         {
            if (star_t* edited_star = dynamic_cast<star_t *>(my_edited_figure.get()))
            {
               return edited_star;
            }
            else if (extended_figure_t* edited_extended = dynamic_cast<extended_figure_t *>(my_edited_figure.get()))
            {
               if (star_t* edited_star = dynamic_cast<star_t *>(edited_extended->child.get()))
                  return edited_star;
            }

            return nullptr;
         }

         rosette_t* get_rosette(std::shared_ptr<figure_t>& my_edited_figure)
         {
            if (rosette_t* edited_rosette = dynamic_cast<rosette_t *>(my_edited_figure.get()))
            {
               return edited_rosette;
            }
            else if (extended_figure_t* edited_extended = dynamic_cast<extended_figure_t *>(my_edited_figure.get()))
            {
               if (rosette_t* edited_rosette = dynamic_cast<rosette_t *>(edited_extended->child.get()))
                  return edited_rosette;
            }

            return nullptr;
         }

         irregular_figure_t* get_irregular_figure(std::shared_ptr<figure_t>& my_edited_figure)
         {
            if (irregular_figure_t* edited_irregular_figure = dynamic_cast<irregular_figure_t *>(my_edited_figure.get()))
            {
               return edited_irregular_figure;
            }
            else if (extended_figure_t* edited_extended = dynamic_cast<extended_figure_t *>(my_edited_figure.get()))
            {
               if (irregular_figure_t* edited_irregular_figure = dynamic_cast<irregular_figure_t *>(edited_extended->child.get()))
                  return edited_irregular_figure;
            }

            return nullptr;
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a figure.

      class figure_editor_ui_t
      {
      public:
         figure_editor_ui_t(figure_editor_t& parent, std::shared_ptr<figure_t> ed)
         : my_figure_editor(parent), my_edited_figure(nullptr)
         {
            build_ui(parent);
            set_edited(ed, false);
         }

         std::shared_ptr<figure_t> get_edited() const
         {
            return my_edited_figure;
         }

         void set_edited(std::shared_ptr<figure_t> ed, bool force_ui_update)
         {
            if (!force_ui_update && ed == my_edited_figure)
               return;

            my_edited_figure = ed;
            fill_ui();
         }

      private:
         void build_ui(figure_editor_t& parent)
         {
            QVBoxLayout* layout = new QVBoxLayout(&parent);
            layout->setContentsMargins(0, 0, 0, 0);

            my_d_editor = new ui::qt::double_editor_t(&parent, L::t(L"Branch Sharpness"), 0, [self=this](double new_value, bool interacting){ self->update_d(new_value, interacting); });
            my_q_editor = new ui::qt::double_editor_t(&parent, L::t(L"Flatness"), 0, [self=this](double new_value, bool interacting){ self->update_q(new_value, interacting); });
            my_s_editor = new ui::qt::int_editor_t(&parent, L::t(L"Intersections"), 0, [self=this](int new_value, bool interacting){ self->update_s(new_value, interacting); });

            my_d_editor->set_limits(-5., 5., 0.01);
            my_q_editor->set_limits(-1., 1., 0.01);
            my_s_editor->set_limits(1, 6);

            my_d_editor->setEnabled(false);
            my_s_editor->setEnabled(false);
            my_q_editor->setEnabled(false);

            layout->addWidget(my_d_editor);
            layout->addWidget(my_q_editor);
            layout->addWidget(my_s_editor);
         }

         void fill_ui()
         {
            my_disable_feedback++;

            if (star_t* edited_star = get_star(my_edited_figure))
            {
               const int sides = edited_star->n;
               my_d_editor->set_value(edited_star->d);
               my_d_editor->set_limits(-sides / 2., sides / 2, 0.01);
               my_s_editor->set_value(edited_star->s);
               my_s_editor->set_limits(1, sides / 2);
            }
            else if (rosette_t* edited_rosette = get_rosette(my_edited_figure))
            {
               const int sides = edited_rosette->n;
               my_q_editor->set_value(edited_rosette->q);
               my_s_editor->set_value(edited_rosette->s);
               my_s_editor->set_limits(1, sides / 2);
            }
            else if (irregular_figure_t* edited_irregular = get_irregular_figure(my_edited_figure))
            {
               const int sides = int(edited_irregular->poly.points.size());
               my_d_editor->set_value(edited_irregular->d);
               my_d_editor->set_limits(-sides / 2., sides / 2, 0.01);
               my_q_editor->set_value(edited_irregular->q);
               my_s_editor->set_value(edited_irregular->s);
               my_s_editor->set_limits(1, sides / 2);
            }

            update_enabled();

            my_disable_feedback--;
         }

         void update_enabled()
         {
            if (star_t* edited_star = get_star(my_edited_figure))
            {
               my_d_editor->setEnabled(true);
               my_s_editor->setEnabled(true);
               my_q_editor->setEnabled(false);
            }
            else if (rosette_t* edited_rosette = get_rosette(my_edited_figure))
            {
               my_d_editor->setEnabled(false);
               my_s_editor->setEnabled(true);
               my_q_editor->setEnabled(true);
            }
            else if (irregular_figure_t* edited_irregular = get_irregular_figure(my_edited_figure))
            {
               switch (edited_irregular->infer)
               {
                  case tiling::infer_mode_t::star:
                     my_d_editor->setEnabled(true);
                     my_s_editor->setEnabled(true);
                     my_q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::girih:
                     my_d_editor->setEnabled(true);
                     my_s_editor->setEnabled(false);
                     my_q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::intersect:
                     my_d_editor->setEnabled(true);
                     my_s_editor->setEnabled(true);
                     my_q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::progressive:
                     my_d_editor->setEnabled(true);
                     my_s_editor->setEnabled(true);
                     my_q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::hourglass:
                     my_d_editor->setEnabled(true);
                     my_s_editor->setEnabled(true);
                     my_q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::rosette:
                  case tiling::infer_mode_t::extended_rosette:
                     my_d_editor->setEnabled(true);
                     my_s_editor->setEnabled(true);
                     my_q_editor->setEnabled(true);
                     break;
                  case tiling::infer_mode_t::simple:
                     my_d_editor->setEnabled(false);
                     my_s_editor->setEnabled(false);
                     my_q_editor->setEnabled(false);
                     break;
               }
            }
            else
            {
               my_d_editor->setEnabled(false);
               my_s_editor->setEnabled(false);
               my_q_editor->setEnabled(false);
            }
         }

         void update_d(double new_value, bool interacting)
         {
            if (my_disable_feedback)
               return;

            if (star_t* edited_star = dynamic_cast<star_t *>(my_edited_figure.get()))
            {
               if (interacting && utility::near(new_value, edited_star->d))
                  return;

               edited_star->d = new_value;
               update(interacting);
            }
           else if (irregular_figure_t* edited_irregular_figure = dynamic_cast<irregular_figure_t *>(my_edited_figure.get()))
            {
               if (interacting && utility::near(new_value, edited_irregular_figure->d))
                  return;

               edited_irregular_figure->d = new_value;
               update(interacting);
            }
         }

         void update_q(double new_value, bool interacting)
         {
            if (my_disable_feedback)
               return;

            if (rosette_t* edited_rosette = get_rosette(my_edited_figure))
            {
               if (interacting && utility::near(new_value, edited_rosette->q))
                  return;

               edited_rosette->q = new_value;
               update(interacting);
            }
            else if (irregular_figure_t* edited_irregular_figure = dynamic_cast<irregular_figure_t *>(my_edited_figure.get()))
            {
               if (interacting && utility::near(new_value, edited_irregular_figure->q))
                  return;

               edited_irregular_figure->q = new_value;
               update(interacting);
            }
         }

         void update_s(int new_value, bool interacting)
         {
            if (my_disable_feedback)
               return;

            if (star_t* edited_star = get_star(my_edited_figure))
            {
               if (interacting && new_value == edited_star->s)
                  return;

               edited_star->s = new_value;
               update(interacting);
            }
            else if (rosette_t* edited_rosette = get_rosette(my_edited_figure))
            {
               if (interacting && new_value == edited_rosette->s)
                  return;

               edited_rosette->s = new_value;
               update(interacting);
            }
            else if (irregular_figure_t* edited_irregular_figure = get_irregular_figure(my_edited_figure))
            {
               if (interacting && new_value == edited_irregular_figure->s)
                  return;

               edited_irregular_figure->s = new_value;
               update(interacting);
            }
         }

         void update(bool interacting)
         {
            if (!my_edited_figure)
               return;

            // Note: used to avoid re-calculating the figure when just setting its value in the UI.
            if (my_disable_feedback)
               return;

            if (my_figure_editor.figure_changed)
               my_figure_editor.figure_changed(my_edited_figure, interacting);
         }

         figure_editor_t& my_figure_editor;
         std::shared_ptr<figure_t> my_edited_figure;

         ui::qt::double_editor_t* my_d_editor;
         ui::qt::int_editor_t* my_s_editor;
         ui::qt::double_editor_t* my_q_editor;

         int my_disable_feedback = 0;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a figure.

      figure_editor_t::figure_editor_t(QWidget* parent)
         : figure_editor_t(parent, nullptr, nullptr)
      {
      }

      figure_editor_t::figure_editor_t(QWidget* parent, figure_changed_callback fc)
         : figure_editor_t(parent, nullptr, fc)
      {
      }

      figure_editor_t::figure_editor_t(QWidget* parent, std::shared_ptr<figure_t> my_edited_figure, figure_changed_callback fc)
      : QWidget(parent), my_ui(new figure_editor_ui_t(*this, my_edited_figure)), figure_changed(fc)
      {
      }

      void figure_editor_t::set_edited(std::shared_ptr<figure_t> my_edited_figure, bool force_ui_update)
      {
         my_ui->set_edited(my_edited_figure, force_ui_update);
      }

      std::shared_ptr<figure_t> figure_editor_t::get_edited() const
      {
         if (!my_ui)
            return nullptr;

         return my_ui->get_edited();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
