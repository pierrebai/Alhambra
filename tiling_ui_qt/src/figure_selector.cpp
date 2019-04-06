#include <dak/tiling_ui_qt/figure_selector.h>
#include <dak/tiling_ui_qt/table_widget_with_combo.h>
#include <dak/tiling_ui_qt/utility.h>

#include <dak/ui_qt/convert.h>

#include <dak/tiling/star.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/irregular_figure.h>
#include <dak/tiling/extended_figure.h>

#include <dak/utility/text.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qheaderview.h>

#include <algorithm>
#include <typeindex>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling::star;
      using tiling::rosette;
      using tiling::irregular_figure;
      using tiling::extended_figure;
      using utility::L;
      typedef std::function<void(std::shared_ptr<figure>)> figure_changed_callback;
      typedef std::function<void(std::shared_ptr<figure>)> selection_changed_callback;
      typedef std::vector<std::shared_ptr<figure>> figures;

      namespace
      {

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
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order figures.

      class figure_selector_ui
      {
      public:
         figure_selector_ui(figure_selector& parent)
         : editor(parent)
         {
            build_ui(parent);
         }

         const figures& get_edited() const
         {
            return edited;
         }

         void set_edited(const figures& ed)
         {
            if (ed == edited)
               return;

            edited = ed;

            const int selected = get_selected_index();

            fill_ui(selected);
         }

         std::shared_ptr<figure> get_selected_figure() const
         {
            const int index = get_selected_index();
            if (0 <= index && index <= edited.size())
               return edited[index];
            return nullptr;
         };

      private:
         void build_ui(figure_selector& parent)
         {
            disable_feedback++;

            QVBoxLayout* layout = new QVBoxLayout(&parent);
            layout->setContentsMargins(0, 0, 0, 0);

            QStringList combo_items;
            for (const auto& infer : infer_modes)
               combo_items.append(QString::fromWCharArray(L::t(tiling::infer_mode_name(infer))));

            figure_list = std::make_unique<table_widget_with_combo>(type_column, combo_items, &parent);
            figure_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
            figure_list->setIconSize(QSize(64, 32));
            figure_list->setColumnCount(2);
            figure_list->setHorizontalHeaderLabels(QStringList(
               {
                  QString::fromWCharArray(L::t(L"Description")),
                  QString::fromWCharArray(L::t(L"Figure")),
               }));
            figure_list->setShowGrid(false);
            figure_list->horizontalHeader()->setSectionResizeMode(description_column, QHeaderView::ResizeMode::Stretch);
            layout->addWidget(figure_list.get());

            figure_list->setEnabled(false);

            figure_list->connect(figure_list.get(), &QTableWidget::itemSelectionChanged, [&]() { update_selection(); });
            figure_list->connect(figure_list.get(), &QTableWidget::itemChanged, [&](QTableWidgetItem * item) { update_infer(item); });

            disable_feedback--;
         }

         void fill_ui(int selected)
         {
            disable_feedback++;
            figure_list->blockSignals(disable_feedback > 0);

            figure_list->setRowCount(0);

            for (auto& figure : edited)
            {
               tiling::infer_mode infer = tiling::infer_mode::girih;
               if (std::dynamic_pointer_cast<extended_figure>(figure))
                  infer = tiling::infer_mode::extended_rosette;
               else if (std::dynamic_pointer_cast<rosette>(figure))
                  infer = tiling::infer_mode::rosette;
               else if (std::dynamic_pointer_cast<star>(figure))
                  infer = tiling::infer_mode::star;
               else if (auto edited_irregular = std::dynamic_pointer_cast<irregular_figure>(figure))
                  infer = edited_irregular->infer;


               const QString type_name = QString::fromWCharArray(L::t(tiling::infer_mode_name(infer)));
               auto desc_item = new QTableWidgetItem(QString::fromWCharArray(figure->describe().c_str()));
               desc_item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
               auto type_item = new QTableWidgetItem(type_name);
               const int row = figure_list->rowCount();
               figure_list->setRowCount(row + 1);
               figure_list->setItem(row, description_column, desc_item);
               figure_list->setItem(row, type_column, type_item);
            }

            figure_list->resizeColumnsToContents();
            figure_list->horizontalHeader()->setSectionResizeMode(description_column, QHeaderView::ResizeMode::Stretch);

            set_selected_index(selected);

            update_enabled();

            disable_feedback--;
            figure_list->blockSignals(disable_feedback > 0);
         }

         void update_enabled()
         {
            auto selected = get_selected_index();

            figure_list->setEnabled(edited.size() > 0);
         }

         void update_selection()
         {
            update_enabled();

            if (disable_feedback)
               return;

            if (editor.selection_changed)
               editor.selection_changed(get_selected_figure());
         }

         void update_infer(tiling::infer_mode new_infer_mode)
         {
            if (disable_feedback)
               return;

            auto figure = get_selected_figure();
            if (!figure)
               return;

            if (auto edited_irregular_figure = get_irregular_figure(figure))
            {
               if (new_infer_mode == edited_irregular_figure->infer)
                  return;

               edited_irregular_figure->infer = new_infer_mode;

               update_enabled();
               fill_ui(get_selected_index());
               update_figure();
            }
            else if (auto edited_radial = std::dynamic_pointer_cast<tiling::radial_figure>(figure))
            {
               auto old_figure = figure;
               switch (new_infer_mode)
               {
               case tiling::infer_mode::star:
                  figure = std::make_shared<star>(edited_radial->n);
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
                  figure = std::make_shared<rosette>(edited_radial->n);
                  break;
               case tiling::infer_mode::extended_rosette:
                  figure = std::make_shared<extended_figure>(std::make_shared<rosette>(edited_radial->n));
                  break;
               case tiling::infer_mode::simple:
                  break;
               }

               figure->make_similar(*old_figure);

               update_enabled();
               update_swap(old_figure, figure);
            }
         }

         void update_infer(QTableWidgetItem * item)
         {
            if (!item)
               return;

            auto new_infer = item->text();
            for (const auto infer : infer_modes)
               if (new_infer == QString::fromWCharArray(L::t(tiling::infer_mode_name(infer))))
                  update_infer(infer);
         }

         void update_figure()
         {
            if (edited.size() <= 0)
               return;

            // Note: used to avoid re-calculating the figure when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.figure_changed)
               editor.figure_changed(get_selected_figure());
         }

         void update_swap(std::shared_ptr<figure> before, std::shared_ptr<figure> after)
         {
            // Note: used to avoid re-calculating the figure when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.figure_swapped)
               editor.figure_swapped(before, after);
         }

         void set_selected_index(const int index)
         {
            figure_list->clearSelection();
            if (index >= 0)
            {
               for (int col = 0; col < figure_list->columnCount(); ++col)
               {
                  const auto item = figure_list->item(index, col);
                  item->setSelected(true);
               }
            }
         }

         int get_selected_index() const
         {
            for (int row = 0; row < figure_list->rowCount() && row < edited.size(); ++row)
            {
               for (int col = 0; col < figure_list->columnCount(); ++col)
               {
                  const auto item = figure_list->item(row, col);
                  if (item->isSelected())
                  {
                     return row;
                  }
               }
            }
            return -1;
         };

         static constexpr int description_column = 0;
         static constexpr int type_column = 1;

         figure_selector& editor;
         figures edited;

         std::unique_ptr<table_widget_with_combo> figure_list;

         int disable_feedback = 0;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order figures.

      figure_selector::figure_selector(QWidget* parent)
      : QWidget(parent), ui(std::make_unique<figure_selector_ui>(*this))
      {
      }

      void figure_selector::set_edited(const figures& edited)
      {
         if (!ui)
            return;

         ui->set_edited(edited);
      }

      const figures& figure_selector::get_edited() const
      {
         static const figures empty;
         if (!ui)
            return empty;

         return ui->get_edited();
      }

      std::shared_ptr<figure> figure_selector::get_selected_figure() const
      {
         if (!ui)
            return {};

         return ui->get_selected_figure();
      }

   }
}

// vim: sw=3 : sts=3 : et : sta : 
