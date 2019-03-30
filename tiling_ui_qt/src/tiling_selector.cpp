#include <dak/tiling_ui_qt/tiling_selector.h>

#include <dak/ui_qt/convert.h>
#include <dak/ui_qt/mosaic_canvas.h>

#include <dak/tiling/known_tilings_generator.h>

#include <dak/geometry/utility.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qdialog.h>

#include <algorithm>

namespace dak
{
   namespace tiling_ui_qt
   {
      using geometry::L;
      using dak::tiling::known_tilings_generator;
      using ui_qt::mosaic_canvas;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order layers.

      class tiling_selector_ui
      {
      public:
         tiling_selector_ui(tiling_selector& parent)
         : editor(parent), tiling_gen(LR"(./tilings)", errors)
         {
            build_ui(parent);
            fill_ui(get_selected_index());
         }

         const std::shared_ptr<mosaic>& get_selected() const
         {
            return tiling_gen.current_mosaic();
         }

      private:
         void build_ui(tiling_selector& parent)
         {
            parent.setWindowTitle(QString::fromWCharArray(L::t(L"Choose a Tiling")));
            QVBoxLayout* layout = new QVBoxLayout(&parent);

            QWidget* selection_panel = new QWidget(&parent);
               QHBoxLayout* selection_layout = new QHBoxLayout(selection_panel);
               tiling_list = std::make_unique<QListWidget>(&parent);
               tiling_list->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
               tiling_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
               selection_layout->addWidget(tiling_list.get());
               tiling_canvas = std::make_unique<mosaic_canvas>(&parent);
               tiling_canvas->setMinimumSize(400, 400);
               selection_layout->addWidget(tiling_canvas.get());
               selection_layout->setStretch(0, 0);
               selection_layout->setStretch(1, 1);
            layout->addWidget(selection_panel);

            QWidget* button_panel = new QWidget(&parent);
               QHBoxLayout* button_layout = new QHBoxLayout(button_panel);
               button_layout->setContentsMargins(0, 0, 0, 0);
               ok_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"OK")), &parent);
               ok_button->setDefault(true);
               button_layout->addWidget(ok_button.get());
               cancel_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Cancel")), &parent);
               button_layout->addWidget(cancel_button.get());
            layout->addWidget(button_panel);

            tiling_list->setEnabled(false);
            ok_button->setEnabled(false);
            cancel_button->setEnabled(false);

            tiling_list->connect(tiling_list.get(), &QListWidget::itemSelectionChanged, [&]() { update_selection(); });
            ok_button->connect(ok_button.get(), &QPushButton::clicked, &parent, &QDialog::accept);
            cancel_button->connect(cancel_button.get(), &QPushButton::clicked, &parent, &QDialog::reject);

            parent.connect(&parent, &QDialog::accepted, [&]() { ok_selection(); });
         }

         void fill_ui(const int selected)
         {
            disable_feedback = true;

            tiling_list->clear();

            for (auto& tiling : tiling_gen.tilings.tilings)
            {
               tiling_list->addItem(QString::fromWCharArray(tiling.name.c_str()));
            }

            set_selected_index(selected);

            update_enabled();

            disable_feedback = false;
         }

         void update_enabled()
         {
            const int selected = get_selected_index();

            tiling_list->setEnabled(true);
            ok_button->setEnabled(selected >= 0);
            cancel_button->setEnabled(true);
         }

         void update_selection()
         {
            const int selected = get_selected_index();
            tiling_gen.set_index(selected);
            if (selected >= 0)
            {
               tiling_canvas->mosaic = tiling_gen.current_mosaic();
               tiling_canvas->repaint();
            }
            update_enabled();
         }

         void ok_selection()
         {
            if (get_selected_index() < 0)
               return;

            // Note: used to avoid re-calculating the UI when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.tiling_chosen)
               editor.tiling_chosen(get_selected());
         }

         void cancel_selection()
         {
            editor.reject();
         }

         void set_selected_index(const int index)
         {
            tiling_list->clearSelection();
            tiling_list->setCurrentRow(index);
         }

         int get_selected_index() const
         {
            return tiling_list->currentRow();
         };

         tiling_selector& editor;
         std::vector<std::wstring> errors;
         known_tilings_generator tiling_gen;

         std::unique_ptr<QListWidget> tiling_list;
         std::unique_ptr<mosaic_canvas> tiling_canvas;
         std::unique_ptr<QPushButton> ok_button;
         std::unique_ptr<QPushButton> cancel_button;

         bool disable_feedback = false;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order layers.

      tiling_selector::tiling_selector(QWidget* parent)
      : tiling_selector(parent, nullptr)
      {
      }

      tiling_selector::tiling_selector(QWidget* parent, tiling_chosen_callback tc)
      : QDialog(parent), ui(std::make_unique<tiling_selector_ui>(*this)), tiling_chosen(tc)
      {
      }

      const std::shared_ptr<mosaic>& tiling_selector::get_selected() const
      {
         if (!ui)
            return nullptr;

         return ui->get_selected();
      }

   }
}

// vim: sw=3 : sts=3 : et : sta : 
