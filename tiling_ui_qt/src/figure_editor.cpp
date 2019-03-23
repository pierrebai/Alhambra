#include <dak/tiling_ui_qt/figure_editor.h>

#include <dak/tiling/star.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/irregular_figure.h>
#include <dak/tiling/extended_figure.h>

#include <dak/ui_qt/int_editor.h>
#include <dak/ui_qt/double_editor.h>

#include <dak/geometry/utility.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qcombobox.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling::star;
      using tiling::rosette;
      using tiling::irregular_figure;
      using tiling::extended_figure;
      using geometry::L;
      typedef std::function<void(std::shared_ptr<figure>)> figure_changed_callback;

      namespace
      {
         star* get_star(std::shared_ptr<figure>& edited)
         {
            if (star* edited_star = dynamic_cast<star *>(edited.get()))
            {
               return edited_star;
            }
            else if (extended_figure* edited_extended = dynamic_cast<extended_figure *>(edited.get()))
            {
               if (star* edited_star = dynamic_cast<star *>(edited_extended->child.get()))
                  return edited_star;
            }

            return nullptr;
         }

         rosette* get_rosette(std::shared_ptr<figure>& edited)
         {
            if (rosette* edited_rosette = dynamic_cast<rosette *>(edited.get()))
            {
               return edited_rosette;
            }
            else if (extended_figure* edited_extended = dynamic_cast<extended_figure *>(edited.get()))
            {
               if (rosette* edited_rosette = dynamic_cast<rosette *>(edited_extended->child.get()))
                  return edited_rosette;
            }

            return nullptr;
         }

         irregular_figure* get_irregular_figure(std::shared_ptr<figure>& edited)
         {
            if (irregular_figure* edited_irregular_figure = dynamic_cast<irregular_figure *>(edited.get()))
            {
               return edited_irregular_figure;
            }
            else if (extended_figure* edited_extended = dynamic_cast<extended_figure *>(edited.get()))
            {
               if (irregular_figure* edited_irregular_figure = dynamic_cast<irregular_figure *>(edited_extended->child.get()))
                  return edited_irregular_figure;
            }

            return nullptr;
         }

         tiling::infer_mode infer_modes[] = 
         {
            tiling::infer_mode::star,
            tiling::infer_mode::girih,
            tiling::infer_mode::intersect,
            tiling::infer_mode::hourglass,
            tiling::infer_mode::rosette,
            tiling::infer_mode::extended_rosette,
            tiling::infer_mode::simple,
         };

         int infer_mode_index(tiling::infer_mode inf)
         {
            return std::find(infer_modes, infer_modes + sizeof(infer_modes) / sizeof(infer_modes[0]), inf) - infer_modes;
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a figure.

      class figure_editor_ui
      {
      public:
         figure_editor_ui(figure_editor& parent, std::shared_ptr<figure> ed)
         : editor(parent), edited(nullptr)
         {
            build_ui(parent);
            set_edited(ed);
         }

         std::shared_ptr<figure> get_edited() const
         {
            return edited;
         }

         void set_edited(std::shared_ptr<figure> ed)
         {
            if (ed == edited)
               return;

            edited = ed;
            fill_ui();
         }

      private:
         void build_ui(figure_editor& parent)
         {
            QVBoxLayout* layout = new QVBoxLayout(&parent);
            layout->setContentsMargins(0, 0, 0, 0);

            infer_mode_editor = std::make_unique<QComboBox>(&parent);
            for (const auto inf : infer_modes)
               infer_mode_editor->addItem(QString::fromWCharArray(L::t(tiling::infer_mode_name(inf))));
            infer_mode_editor->connect(infer_mode_editor.get(), &QComboBox::currentTextChanged, [&](const QString& text){
               for (const auto inf : infer_modes)
               {
                  if (text == QString::fromWCharArray(L::t(tiling::infer_mode_name(inf))))
                  {
                     update_infer(inf);
                  }
               }
            });

            d_editor = std::make_unique<ui_qt::double_editor>(&parent, L::t(L"Branch Sharpness"), 0, std::bind(&figure_editor_ui::update_d, this, std::placeholders::_1));
            q_editor = std::make_unique<ui_qt::double_editor>(&parent, L::t(L"Flatness"), 0, std::bind(&figure_editor_ui::update_q, this, std::placeholders::_1));
            s_editor = std::make_unique<ui_qt::int_editor>(&parent, L::t(L"Intersections"), 0, std::bind(&figure_editor_ui::update_s, this, std::placeholders::_1));

            d_editor->set_limits(-5., 5., 0.05);
            q_editor->set_limits(-1., 1., 0.01);
            s_editor->set_limits(1, 6);

            infer_mode_editor->setEnabled(false);
            d_editor->setEnabled(false);
            s_editor->setEnabled(false);
            q_editor->setEnabled(false);

            layout->addWidget(infer_mode_editor.get());
            layout->addWidget(d_editor.get());
            layout->addWidget(q_editor.get());
            layout->addWidget(s_editor.get());
         }

         void fill_ui()
         {
            disable_feedback++;

            if (star* edited_star = get_star(edited))
            {
               d_editor->set_value(edited_star->d);
               s_editor->set_value(edited_star->s);
            }
            else if (rosette* edited_rosette = get_rosette(edited))
            {
               q_editor->set_value(edited_rosette->q);
               s_editor->set_value(edited_rosette->s);
            }
            else if (irregular_figure* edited_irregular = get_irregular_figure(edited))
            {
               d_editor->set_value(edited_irregular->d);
               q_editor->set_value(edited_irregular->q);
               s_editor->set_value(edited_irregular->s);
            }

            fill_infer_ui();

            update_enabled();

            disable_feedback--;
         }

         void fill_infer_ui()
         {
            disable_feedback++;

            tiling::infer_mode infer = tiling::infer_mode::girih;
            if (std::dynamic_pointer_cast<extended_figure>(edited))
               infer = tiling::infer_mode::extended_rosette;
            else if (std::dynamic_pointer_cast<rosette>(edited))
               infer = tiling::infer_mode::rosette;
            else if (std::dynamic_pointer_cast<star>(edited))
               infer = tiling::infer_mode::star;
            else if (auto edited_irregular = std::dynamic_pointer_cast<irregular_figure>(edited))
               infer = edited_irregular->infer;

            const int new_index = infer_mode_index(infer);
            if (new_index != infer_mode_editor->currentIndex())
               infer_mode_editor->setCurrentIndex(new_index);

            disable_feedback--;
         }

         void update_enabled()
         {
            if (star* edited_star = get_star(edited))
            {
               infer_mode_editor->setEnabled(true);
               d_editor->setEnabled(true);
               s_editor->setEnabled(true);
               q_editor->setEnabled(false);
            }
            else if (rosette* edited_rosette = get_rosette(edited))
            {
               infer_mode_editor->setEnabled(true);
               d_editor->setEnabled(false);
               s_editor->setEnabled(true);
               q_editor->setEnabled(true);
            }
            else if (irregular_figure* edited_irregular = get_irregular_figure(edited))
            {
               infer_mode_editor->setEnabled(true);
               switch (edited_irregular->infer)
               {
                  case tiling::infer_mode::star:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode::girih:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(false);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode::intersect:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode::progressive:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode::hourglass:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(false);
                     break;
                  case tiling::infer_mode::rosette:
                  case tiling::infer_mode::extended_rosette:
                     d_editor->setEnabled(true);
                     s_editor->setEnabled(true);
                     q_editor->setEnabled(true);
                     break;
                  case tiling::infer_mode::simple:
                     d_editor->setEnabled(false);
                     s_editor->setEnabled(false);
                     q_editor->setEnabled(false);
                     break;
               }
            }
            else
            {
               infer_mode_editor->setEnabled(false);
               d_editor->setEnabled(false);
               s_editor->setEnabled(false);
               q_editor->setEnabled(false);
            }
         }

         void update_d(double new_value)
         {
            if (disable_feedback)
               return;

            if (star* edited_star = dynamic_cast<star *>(edited.get()))
            {
               if (geometry::near(new_value, edited_star->d))
                  return;

               edited_star->d = new_value;
               update();
            }
           else if (irregular_figure* edited_irregular_figure = dynamic_cast<irregular_figure *>(edited.get()))
            {
               if (geometry::near(new_value, edited_irregular_figure->d))
                  return;

               edited_irregular_figure->d = new_value;
               update();
            }
         }

         void update_q(double new_value)
         {
            if (disable_feedback)
               return;

            if (rosette* edited_rosette = get_rosette(edited))
            {
               if (geometry::near(new_value, edited_rosette->q))
                  return;

               edited_rosette->q = new_value;
               update();
            }
            else if (irregular_figure* edited_irregular_figure = dynamic_cast<irregular_figure *>(edited.get()))
            {
               if (geometry::near(new_value, edited_irregular_figure->q))
                  return;

               edited_irregular_figure->q = new_value;
               update();
            }
         }

         void update_s(int new_value)
         {
            if (disable_feedback)
               return;

            if (star* edited_star = get_star(edited))
            {
               if (new_value == edited_star->s)
                  return;

               edited_star->s = new_value;
               update();
            }
            else if (rosette* edited_rosette = get_rosette(edited))
            {
               if (new_value == edited_rosette->s)
                  return;

               edited_rosette->s = new_value;
               update();
            }
            else if (irregular_figure* edited_irregular_figure = get_irregular_figure(edited))
            {
               if (new_value == edited_irregular_figure->s)
                  return;

               edited_irregular_figure->s = new_value;
               update();
            }
         }

         void update_infer(tiling::infer_mode new_infer_mode)
         {
            if (disable_feedback)
               return;

            if (!edited)
               return;

            if (auto edited_irregular_figure = get_irregular_figure(edited))
            {
               if (new_infer_mode == edited_irregular_figure->infer)
                  return;

               edited_irregular_figure->infer = new_infer_mode;

               update_enabled();
               fill_infer_ui();
               update();
            }
            else if (auto edited_radial = std::dynamic_pointer_cast<tiling::radial_figure>(edited))
            {
               auto old_edited = edited;
               switch (new_infer_mode)
               {
                  case tiling::infer_mode::star:
                     edited = std::make_shared<star>(edited_radial->n);
                     break;
                  case tiling::infer_mode::girih:
                     break;
                  case tiling::infer_mode::intersect:
                     break;
                  case tiling::infer_mode::progressive:
                     break;
                  case tiling::infer_mode::hourglass:
                     break;
                  case tiling::infer_mode::rosette:
                     edited = std::make_shared<rosette>(edited_radial->n);
                     break;
                  case tiling::infer_mode::extended_rosette:
                     edited = std::make_shared<extended_figure>(std::make_shared<rosette>(edited_radial->n));
                     break;
                  case tiling::infer_mode::simple:
                     break;
               }

               edited->make_similar(*old_edited);

               update_enabled();
               fill_infer_ui();
               update_swap(old_edited);
            }
         }

         void update()
         {
            if (!edited)
               return;

            // Note: used to avoid re-calculating the figure when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.figure_changed)
               editor.figure_changed(edited);
         }

         void update_swap(std::shared_ptr<figure> before)
         {
            if (!edited)
               return;

            // Note: used to avoid re-calculating the figure when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.figure_swapped)
               editor.figure_swapped(before, edited);
         }

         figure_editor& editor;
         std::shared_ptr<figure> edited;

         std::unique_ptr<QComboBox> infer_mode_editor;
         std::unique_ptr<ui_qt::double_editor> d_editor;
         std::unique_ptr<ui_qt::int_editor> s_editor;
         std::unique_ptr<ui_qt::double_editor> q_editor;

         int disable_feedback = 0;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to edit a figure.

      figure_editor::figure_editor(QWidget* parent)
         : figure_editor(parent, nullptr, nullptr)
      {
      }

      figure_editor::figure_editor(QWidget* parent, figure_changed_callback fc)
         : figure_editor(parent, nullptr, fc)
      {
      }

      figure_editor::figure_editor(QWidget* parent, std::shared_ptr<figure> edited, figure_changed_callback fc)
      : QWidget(parent), ui(std::make_unique<figure_editor_ui>(*this, edited)), figure_changed(fc)
      {
      }

      void figure_editor::set_edited(std::shared_ptr<figure> edited)
      {
         ui->set_edited(edited);
      }

      std::shared_ptr<figure> figure_editor::get_edited() const
      {
         if (!ui)
            return nullptr;

         return ui->get_edited();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
