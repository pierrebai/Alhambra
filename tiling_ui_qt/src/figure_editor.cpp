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
         star_t* get_star(std::shared_ptr<figure_t>& edited)
         {
            if (star_t* edited_star = dynamic_cast<star_t *>(edited.get()))
            {
               return edited_star;
            }
            else if (extended_figure_t* edited_extended = dynamic_cast<extended_figure_t *>(edited.get()))
            {
               if (star_t* edited_star = dynamic_cast<star_t *>(edited_extended->child.get()))
                  return edited_star;
            }

            return nullptr;
         }

         rosette_t* get_rosette(std::shared_ptr<figure_t>& edited)
         {
            if (rosette_t* edited_rosette = dynamic_cast<rosette_t *>(edited.get()))
            {
               return edited_rosette;
            }
            else if (extended_figure_t* edited_extended = dynamic_cast<extended_figure_t *>(edited.get()))
            {
               if (rosette_t* edited_rosette = dynamic_cast<rosette_t *>(edited_extended->child.get()))
                  return edited_rosette;
            }

            return nullptr;
         }

         irregular_figure_t* get_irregular_figure(std::shared_ptr<figure_t>& edited)
         {
            if (irregular_figure_t* edited_irregular_figure = dynamic_cast<irregular_figure_t *>(edited.get()))
            {
               return edited_irregular_figure;
            }
            else if (extended_figure_t* edited_extended = dynamic_cast<extended_figure_t *>(edited.get()))
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
         : editor(parent), edited(nullptr)
         {
            build_ui(parent);
            set_edited(ed, false);
         }

         std::shared_ptr<figure_t> get_edited() const
         {
            return edited;
         }

         void set_edited(std::shared_ptr<figure_t> ed, bool force_ui_update)
         {
            if (!force_ui_update && ed == edited)
               return;

            edited = ed;
            fill_ui();
         }

      private:
         void build_ui(figure_editor_t& parent)
         {
            QVBoxLayout* layout = new QVBoxLayout(&parent);
            layout->setContentsMargins(0, 0, 0, 0);

            d_editor = std::make_unique<ui::qt::double_editor_t>(&parent, L::t(L"Branch Sharpness"), 0, [self=this](double new_value, bool interacting){ self->update_d(new_value, interacting); });
            q_editor = std::make_unique<ui::qt::double_editor_t>(&parent, L::t(L"Flatness"), 0, [self=this](double new_value, bool interacting){ self->update_q(new_value, interacting); });
            s_editor = std::make_unique<ui::qt::int_editor_t>(&parent, L::t(L"Intersections"), 0, [self=this](int new_value, bool interacting){ self->update_s(new_value, interacting); });

            d_editor->set_limits(-5., 5., 0.05);
            q_editor->set_limits(-1., 1., 0.01);
            s_editor->set_limits(1, 6);

            d_editor->setEnabled(false);
            s_editor->setEnabled(false);
            q_editor->setEnabled(false);

            layout->addWidget(d_editor.get());
            layout->addWidget(q_editor.get());
            layout->addWidget(s_editor.get());
         }

         void fill_ui()
         {
            disable_feedback++;

            if (star_t* edited_star = get_star(edited))
            {
               const int sides = edited_star->n;
               d_editor->set_value(edited_star->d);
               d_editor->set_limits(-sides / 2., sides / 2, 0.05);
               s_editor->set_value(edited_star->s);
               s_editor->set_limits(1, sides / 2);
            }
            else if (rosette_t* edited_rosette = get_rosette(edited))
            {
               const int sides = edited_rosette->n;
               q_editor->set_value(edited_rosette->q);
               s_editor->set_value(edited_rosette->s);
               s_editor->set_limits(1, sides / 2);
            }
            else if (irregular_figure_t* edited_irregular = get_irregular_figure(edited))
            {
               const int sides = int(edited_irregular->poly.points.size());
               d_editor->set_value(edited_irregular->d);
               d_editor->set_limits(-sides / 2., sides / 2, 0.05);
               q_editor->set_value(edited_irregular->q);
               s_editor->set_value(edited_irregular->s);
               s_editor->set_limits(1, sides / 2);
            }

            update_enabled();

            disable_feedback--;
         }

         void update_enabled()
         {
            if (star_t* edited_star = get_star(edited))
            {
               d_editor->setEnabled(true);
               s_editor->setEnabled(true);
               q_editor->setEnabled(false);
            }
            else if (rosette_t* edited_rosette = get_rosette(edited))
            {
               d_editor->setEnabled(false);
               s_editor->setEnabled(true);
               q_editor->setEnabled(true);
            }
            else if (irregular_figure_t* edited_irregular = get_irregular_figure(edited))
            {
               switch (edited_irregular->infer)
               {
                  case tiling::infer_mode_t::star:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::girih:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(false);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::intersect:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::progressive:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::hourglass:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode_t::rosette:
                  case tiling::infer_mode_t::extended_rosette:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(true);
                     break;
                  case tiling::infer_mode_t::simple:
                     d_editor->setEnabled(false);
                     s_editor->setEnabled(false);
                     q_editor->setEnabled(false);
                     break;
               }
            }
            else
            {
               d_editor->setEnabled(false);
               s_editor->setEnabled(false);
               q_editor->setEnabled(false);
            }
         }

         void update_d(double new_value, bool interacting)
         {
            if (disable_feedback)
               return;

            if (star_t* edited_star = dynamic_cast<star_t *>(edited.get()))
            {
               if (interacting && utility::near(new_value, edited_star->d))
                  return;

               edited_star->d = new_value;
               update(interacting);
            }
           else if (irregular_figure_t* edited_irregular_figure = dynamic_cast<irregular_figure_t *>(edited.get()))
            {
               if (interacting && utility::near(new_value, edited_irregular_figure->d))
                  return;

               edited_irregular_figure->d = new_value;
               update(interacting);
            }
         }

         void update_q(double new_value, bool interacting)
         {
            if (disable_feedback)
               return;

            if (rosette_t* edited_rosette = get_rosette(edited))
            {
               if (interacting && utility::near(new_value, edited_rosette->q))
                  return;

               edited_rosette->q = new_value;
               update(interacting);
            }
            else if (irregular_figure_t* edited_irregular_figure = dynamic_cast<irregular_figure_t *>(edited.get()))
            {
               if (interacting && utility::near(new_value, edited_irregular_figure->q))
                  return;

               edited_irregular_figure->q = new_value;
               update(interacting);
            }
         }

         void update_s(int new_value, bool interacting)
         {
            if (disable_feedback)
               return;

            if (star_t* edited_star = get_star(edited))
            {
               if (interacting && new_value == edited_star->s)
                  return;

               edited_star->s = new_value;
               update(interacting);
            }
            else if (rosette_t* edited_rosette = get_rosette(edited))
            {
               if (interacting && new_value == edited_rosette->s)
                  return;

               edited_rosette->s = new_value;
               update(interacting);
            }
            else if (irregular_figure_t* edited_irregular_figure = get_irregular_figure(edited))
            {
               if (interacting && new_value == edited_irregular_figure->s)
                  return;

               edited_irregular_figure->s = new_value;
               update(interacting);
            }
         }

         void update(bool interacting)
         {
            if (!edited)
               return;

            // Note: used to avoid re-calculating the figure when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.figure_changed)
               editor.figure_changed(edited, interacting);
         }

         figure_editor_t& editor;
         std::shared_ptr<figure_t> edited;

         std::unique_ptr<ui::qt::double_editor_t> d_editor;
         std::unique_ptr<ui::qt::int_editor_t> s_editor;
         std::unique_ptr<ui::qt::double_editor_t> q_editor;

         int disable_feedback = 0;
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

      figure_editor_t::figure_editor_t(QWidget* parent, std::shared_ptr<figure_t> edited, figure_changed_callback fc)
      : QWidget(parent), ui(std::make_unique<figure_editor_ui_t>(*this, edited)), figure_changed(fc)
      {
      }

      void figure_editor_t::set_edited(std::shared_ptr<figure_t> edited, bool force_ui_update)
      {
         ui->set_edited(edited, force_ui_update);
      }

      std::shared_ptr<figure_t> figure_editor_t::get_edited() const
      {
         if (!ui)
            return nullptr;

         return ui->get_edited();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
