#include <dak/tiling_ui_qt/tiling_selector.h>

#include <dak/ui_qt/convert.h>
#include <dak/ui_qt/mosaic_canvas.h>
#include <dak/ui_qt/tiling_canvas.h>

#include <dak/tiling/known_tilings_generator.h>

#include <dak/utility/text.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qdialogbuttonbox.h>
#include <QtWidgets/qtabbar.h>
#include <QtWidgets/qstackedwidget.h>

#include <algorithm>

namespace dak
{
   namespace tiling_ui_qt
   {
      using utility::L;
      using dak::tiling::known_tilings_generator;
      using ui_qt::mosaic_canvas;
      using ui_qt::tiling_canvas;

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
            layout->setContentsMargins(8, 0, 8, 0);
            layout->setSpacing(0);

            QWidget* header_panel = new QWidget(&parent);
               QHBoxLayout* header_layout = new QHBoxLayout(header_panel);
               header_layout->setSpacing(0);
               header_layout->setContentsMargins(8, 8, 8, 0);

               tiling_description = std::make_unique<QLabel>(QString::fromWCharArray(L::t(L"No tiling selected.")), header_panel);
               tiling_description->setWordWrap(true);
               tiling_description->setMinimumHeight(tiling_description->fontMetrics().height() * 5);
               tiling_description->setMinimumWidth(tiling_description->fontMetrics().averageCharWidth() * 90);
               header_layout->addWidget(tiling_description.get(), 1, Qt::AlignLeft | Qt::AlignTop);

               canvas_tab = std::make_unique<QTabBar>(header_panel);
               canvas_tab->addTab(QString::fromWCharArray(L::t(L"Mosaic Example")));
               canvas_tab->addTab(QString::fromWCharArray(L::t(L"Tile Polygons")));
               header_layout->addWidget(canvas_tab.get(), 0, Qt::AlignRight | Qt::AlignBottom);

            layout->addWidget(header_panel);

            QWidget* selection_panel = new QWidget(&parent);
               QHBoxLayout* selection_layout = new QHBoxLayout(selection_panel);
               selection_layout->setSpacing(8);
               selection_layout->setContentsMargins(8, 0, 8, 8);

               tiling_list = std::make_unique<QListWidget>(selection_panel);
               tiling_list->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
               tiling_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
               selection_layout->addWidget(tiling_list.get());

               stacked_canvas = std::make_unique<QStackedWidget>(selection_panel);
                  stacked_canvas->setContentsMargins(0, 0, 0, 0);

                  example_canvas = std::make_unique<mosaic_canvas>(stacked_canvas.get());
                  example_canvas->setMinimumSize(400, 400);
                  stacked_canvas->addWidget(example_canvas.get());

                  raw_tiling_canvas = std::make_unique<tiling_canvas>(stacked_canvas.get());
                  raw_tiling_canvas->setMinimumSize(400, 400);
                  stacked_canvas->addWidget(raw_tiling_canvas.get());

               selection_layout->addWidget(stacked_canvas.get());

               selection_layout->setStretch(0, 0);
               selection_layout->setStretch(1, 1);

            layout->addWidget(selection_panel);

            dialog_buttons = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
            dialog_buttons->setContentsMargins(8, 8, 8, 8);
            layout->addWidget(dialog_buttons.get());

            tiling_list->setEnabled(false);
            dialog_buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
            dialog_buttons->button(QDialogButtonBox::Cancel)->setEnabled(false);

            tiling_list->connect(tiling_list.get(), &QListWidget::itemSelectionChanged, [&]() { update_selection(); });
            dialog_buttons->connect(dialog_buttons.get(), &QDialogButtonBox::accepted, &parent, &QDialog::accept);
            dialog_buttons->connect(dialog_buttons.get(), &QDialogButtonBox::rejected, &parent, &QDialog::reject);
            canvas_tab->connect(canvas_tab.get(), &QTabBar::currentChanged, stacked_canvas.get(), &QStackedWidget::setCurrentIndex);

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
            dialog_buttons->button(QDialogButtonBox::Ok)->setEnabled(selected >= 0);
            dialog_buttons->button(QDialogButtonBox::Cancel)->setEnabled(true);
         }

         void update_selection()
         {
            const int selected = get_selected_index();
            tiling_gen.set_index(selected);
            if (selected >= 0)
            {
               example_canvas->mosaic = tiling_gen.current_mosaic();
               example_canvas->repaint();

               raw_tiling_canvas->tiling = tiling_gen.current_mosaic()->tiling;
               raw_tiling_canvas->repaint();

               const auto& tiling = tiling_gen.tilings.tilings[selected];
               tiling_description->setTextFormat(Qt::TextFormat::RichText);
               tiling_description->setText(QString::asprintf(R"(%ls<p><hr><i><b>%ls</b></i>)", tiling.description.c_str(), tiling.author.c_str()));
            }
            else
            {
               tiling_description->setText(QString::fromWCharArray(L::t(L"No tiling selected.")));
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

         std::unique_ptr<QLabel> tiling_description;
         std::unique_ptr<QTabBar> canvas_tab;

         std::unique_ptr<QListWidget> tiling_list;
         std::unique_ptr<QStackedWidget> stacked_canvas;
         std::unique_ptr<mosaic_canvas> example_canvas;
         std::unique_ptr<tiling_canvas> raw_tiling_canvas;

         std::unique_ptr<QDialogButtonBox> dialog_buttons;

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
