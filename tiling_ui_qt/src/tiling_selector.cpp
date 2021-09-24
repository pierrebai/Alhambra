#include <dak/tiling_ui_qt/tiling_selector.h>
#include <dak/tiling_ui_qt/tiling_editor.h>
#include <dak/tiling_ui_qt/ask.h>
#include <dak/tiling_ui_qt/mosaic_canvas.h>
#include <dak/tiling_ui_qt/tiling_canvas.h>

#include <dak/ui/qt/convert.h>

#include <dak/tiling/known_tilings.h>

#include <dak/utility/text.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qdialogbuttonbox.h>
#include <QtWidgets/qtabbar.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qstackedwidget.h>

#include <algorithm>

namespace dak
{
   namespace tiling_ui_qt
   {
      using utility::L;
      using namespace dak::QtAdditions;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select a tiling.

      class tiling_selector_ui_t
      {
      public:
         tiling_selector_ui_t(known_tilings_t& known_tilings, const tiling_editor_icons_t& icons, tiling_selector_t& parent)
         : my_tiling_selector(parent), my_known_tilings(known_tilings), my_reporter(&parent)
         {
            build_ui(icons, parent);
            fill_ui(get_selected_index());
         }

         const std::shared_ptr<tiling_t> get_selected() const
         {
            return my_raw_tiling_canvas->tiling;
         }

      private:
         void build_ui(const tiling_editor_icons_t& icons, tiling_selector_t& parent)
         {
            parent.setWindowTitle(QString::fromWCharArray(L::t(L"Choose a Tiling")));
            QVBoxLayout* layout = new QVBoxLayout(&parent);
            layout->setContentsMargins(8, 0, 8, 0);
            layout->setSpacing(0);

            QWidget* header_panel = new QWidget(&parent);
               QHBoxLayout* header_layout = new QHBoxLayout(header_panel);
               header_layout->setSpacing(0);
               header_layout->setContentsMargins(8, 8, 8, 0);

               my_tiling_description = std::make_unique<QLabel>(QString::fromWCharArray(L::t(L"No tiling selected.")), header_panel);
               my_tiling_description->setWordWrap(true);
               my_tiling_description->setMinimumHeight(my_tiling_description->fontMetrics().height() * 5);
               my_tiling_description->setMinimumWidth(my_tiling_description->fontMetrics().averageCharWidth() * 90);
               header_layout->addWidget(my_tiling_description.get(), 1, Qt::AlignLeft | Qt::AlignTop);

               my_canvas_tab = std::make_unique<QTabBar>(header_panel);
               my_canvas_tab->addTab(QString::fromWCharArray(L::t(L"Mosaic Example")));
               my_canvas_tab->addTab(QString::fromWCharArray(L::t(L"Tile Polygons")));
               header_layout->addWidget(my_canvas_tab.get(), 0, Qt::AlignRight | Qt::AlignBottom);

            layout->addWidget(header_panel);

            QWidget* selection_panel = new QWidget(&parent);
               QHBoxLayout* selection_layout = new QHBoxLayout(selection_panel);
               selection_layout->setSpacing(8);
               selection_layout->setContentsMargins(8, 0, 8, 8);

               QWidget* list_panel = new QWidget(&parent);
                  QVBoxLayout* list_layout = new QVBoxLayout(list_panel);
                  list_layout->setSpacing(0);
                  list_layout->setContentsMargins(0, 0, 0, 0);

                  QToolBar* toolbar = new QToolBar;

                     my_open_action = CreateToolButton(L::t(L"Open..."), icons.tiling_open, 'O', L::t(L"Open a tiling design. (Shortcut: o)"), [self = this]()
                     {
                        self->open_tiling();
                     });
                     toolbar->addWidget(my_open_action);

                  list_layout->addWidget(toolbar);

                  my_tiling_list = std::make_unique<QListWidget>(selection_panel);
                  my_tiling_list->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
                  my_tiling_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
                  list_layout->addWidget(my_tiling_list.get());

               selection_layout->addWidget(list_panel);

               my_stacked_canvas = std::make_unique<QStackedWidget>(selection_panel);
                  my_stacked_canvas->setContentsMargins(0, 0, 0, 0);

                  my_example_canvas = std::make_unique<mosaic_canvas_t>(my_stacked_canvas.get());
                  my_example_canvas->setMinimumSize(400, 400);
                  my_stacked_canvas->addWidget(my_example_canvas.get());

                  my_raw_tiling_canvas = std::make_unique<tiling_canvas_t>(my_stacked_canvas.get());
                  my_raw_tiling_canvas->setMinimumSize(400, 400);
                  my_stacked_canvas->addWidget(my_raw_tiling_canvas.get());

               selection_layout->addWidget(my_stacked_canvas.get());

               selection_layout->setStretch(0, 0);
               selection_layout->setStretch(1, 1);

            layout->addWidget(selection_panel);

            my_dialog_buttons = std::make_unique<QDialogButtonBox>(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
            my_dialog_buttons->setContentsMargins(8, 8, 8, 8);
            layout->addWidget(my_dialog_buttons.get());

            my_tiling_list->setEnabled(false);
            my_dialog_buttons->button(QDialogButtonBox::Ok)->setEnabled(false);
            my_dialog_buttons->button(QDialogButtonBox::Cancel)->setEnabled(false);

            my_tiling_list->connect(my_tiling_list.get(), &QListWidget::itemSelectionChanged, [&]() { update_selection(); });
            my_tiling_list->connect(my_tiling_list.get(), &QListWidget::itemDoubleClicked, &parent, &QDialog::accept);
            my_dialog_buttons->connect(my_dialog_buttons.get(), &QDialogButtonBox::accepted, &parent, &QDialog::accept);
            my_dialog_buttons->connect(my_dialog_buttons.get(), &QDialogButtonBox::rejected, &parent, &QDialog::reject);
            my_canvas_tab->connect(my_canvas_tab.get(), &QTabBar::currentChanged, my_stacked_canvas.get(), &QStackedWidget::setCurrentIndex);

            parent.connect(&parent, &QDialog::accepted, [&]() { ok_selection(); });
         }

         void fill_ui(const int selected)
         {
            my_disable_feedback++;
            my_tiling_list->blockSignals(my_disable_feedback > 0);

            my_tiling_list->clear();

            for (auto& name_and_tiling : my_known_tilings)
            {
               my_tiling_list->addItem(QString::fromWCharArray(name_and_tiling.second->name.c_str()));
            }

            set_selected_index(selected);

            update_enabled();

            my_disable_feedback--;
            my_tiling_list->blockSignals(my_disable_feedback > 0);
         }

         void set_tiling(const std::shared_ptr<tiling_t>& tiling, const std::filesystem::path& path)
         {
            my_disable_feedback++;
            my_tiling_list->blockSignals(my_disable_feedback > 0);

            auto name = add_tiling(my_known_tilings, tiling, path);
            my_tiling_list->addItem(QString::fromWCharArray(name.c_str()));

            set_selected_index(int(my_known_tilings.size() - 1));

            update_enabled();

            my_disable_feedback--;
            my_tiling_list->blockSignals(my_disable_feedback > 0);

            update_selection();
         }

         void update_enabled()
         {
            const int selected = get_selected_index();

            my_tiling_list->setEnabled(true);
            my_dialog_buttons->button(QDialogButtonBox::Ok)->setEnabled(selected >= 0);
            my_dialog_buttons->button(QDialogButtonBox::Cancel)->setEnabled(true);
         }

         void update_selection()
         {
            const int selected = get_selected_index();
            if (selected >= 0 && selected < my_known_tilings.size())
            {
               auto iter = my_known_tilings.begin();
               std::advance(iter, selected);
               const auto& tiling = iter->second;

               my_example_canvas->mosaic = dak::tiling::generate_mosaic(tiling);
               my_example_canvas->repaint();

               my_raw_tiling_canvas->tiling = tiling;
               my_raw_tiling_canvas->repaint();

               my_tiling_description->setTextFormat(Qt::TextFormat::RichText);
               my_tiling_description->setText(QString::asprintf(R"(%ls<p><hr><i><b>%ls</b></i>)", tiling->description.c_str(), tiling->author.c_str()));
            }
            else
            {
               my_tiling_description->setText(QString::fromWCharArray(L::t(L"No tiling selected.")));
            }

            update_enabled();
         }

         void ok_selection()
         {
            if (get_selected_index() < 0)
               return;

            // Note: used to avoid re-calculating the UI when just setting its value in the UI.
            if (my_disable_feedback)
               return;

            if (my_tiling_selector.tiling_chosen)
               my_tiling_selector.tiling_chosen(get_selected());
         }

         void cancel_selection()
         {
            my_tiling_selector.reject();
         }

         void set_selected_index(const int index)
         {
            my_tiling_list->clearSelection();
            my_tiling_list->setCurrentRow(index);
         }

         int get_selected_index() const
         {
            return my_tiling_list->currentRow();
         };

         void open_tiling()
         {
            try
            {
               std::filesystem::path path;
               auto tiling = tiling_ui_qt::ask_open_tiling(path, &my_tiling_selector);
               if (!tiling || tiling->is_invalid())
                  return;
               set_tiling(tiling, path);
            }
            catch (const std::exception& e)
            {
               const std::wstring error_msg = utility::convert(e.what());
               const std::wstring error = error_msg.length() > 0
                  ? std::wstring(L::t(L"Reason given: ")) + error_msg + std::wstring(L::t(L"."))
                  : std::wstring(L::t(L"No reason given for the error."));
               my_reporter.report(std::wstring(L::t(L"Cannot read the selected tiling!\n")) + error, message_reporter_t::category_t::error);
            }
         }

         tiling_selector_t& my_tiling_selector;
         known_tilings_t my_known_tilings;

         std::unique_ptr<QLabel> my_tiling_description;
         std::unique_ptr<QTabBar> my_canvas_tab;

         QToolButton* my_open_action = nullptr;

         std::unique_ptr<QListWidget> my_tiling_list;
         std::unique_ptr<QStackedWidget> my_stacked_canvas;
         std::unique_ptr<mosaic_canvas_t> my_example_canvas;
         std::unique_ptr<tiling_canvas_t> my_raw_tiling_canvas;

         std::unique_ptr<QDialogButtonBox> my_dialog_buttons;

         int my_disable_feedback = 0;

         message_reporter_t my_reporter;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order layers.

      tiling_selector_t::tiling_selector_t(known_tilings_t& known_tilings, const tiling_editor_icons_t& icons, QWidget* parent)
      : tiling_selector_t(known_tilings, icons, parent, nullptr)
      {
      }

      tiling_selector_t::tiling_selector_t(known_tilings_t& known_tilings, const tiling_editor_icons_t& icons, QWidget* parent, tiling_chosen_callback tc)
      : QDialog(parent), ui(std::make_unique<tiling_selector_ui_t>(known_tilings, icons, *this)), tiling_chosen(tc)
      {
      }

      std::shared_ptr<tiling_t> tiling_selector_t::get_selected() const
      {
         if (!ui)
            return nullptr;

         return ui->get_selected();
      }

   }
}

// vim: sw=3 : sts=3 : et : sta : 
