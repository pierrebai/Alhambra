#include <dak/tiling_ui_qt/layers_selector.h>
#include <dak/tiling_ui_qt/table_widget_with_combo.h>
#include <dak/tiling_ui_qt/utility.h>

#include <dak/ui_qt/convert.h>

#include <dak/tiling_style/plain.h>
#include <dak/tiling_style/thick.h>
#include <dak/tiling_style/sketch.h>
#include <dak/tiling_style/outline.h>
#include <dak/tiling_style/emboss.h>
#include <dak/tiling_style/filled.h>
#include <dak/tiling_style/interlace.h>

#include <dak/utility/text.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qheaderview.h>

#include <algorithm>
#include <typeindex>

namespace dak
{
   namespace tiling_ui_qt
   {
      using tiling_style::style;
      using tiling_style::plain;
      using tiling_style::colored;
      using tiling_style::thick;
      using tiling_style::sketch;
      using tiling_style::outline;
      using tiling_style::emboss;
      using tiling_style::filled;
      using tiling_style::interlace;

      using utility::L;
      typedef std::vector<std::shared_ptr<layer>> layers;
      typedef std::function<void(const layers& )> selection_changed_callback;

      namespace
      {
         std::shared_ptr<style> make_plain()
         {
            return std::make_shared<plain>();
         }

         std::shared_ptr<style> make_thick()
         {
            return std::make_shared<thick>();
         }

         std::shared_ptr<style> make_sketch()
         {
            return std::make_shared<sketch>();
         }

         std::shared_ptr<style> make_outline()
         {
            return std::make_shared<outline>();
         }

         std::shared_ptr<style> make_emboss()
         {
            return std::make_shared<emboss>();
         }

         std::shared_ptr<style> make_filled()
         {
            return std::make_shared<filled>();
         }

         std::shared_ptr<style> make_interlace()
         {
            return std::make_shared<interlace>();
         }

         struct
         {
            std::type_index type;
            const wchar_t* name;
            std::shared_ptr<style>(*maker)();
         }
         style_names[] =
         {
            { typeid(plain), L"Plain", make_plain },
            { typeid(thick), L"Thick", make_thick },
            { typeid(sketch), L"Sketched", make_sketch },
            { typeid(outline), L"Outlined", make_outline },
            { typeid(emboss), L"Embossed", make_emboss },
            { typeid(filled), L"Filled", make_filled },
            { typeid(interlace), L"Interlaced", make_interlace },
         };

         const wchar_t* get_style_name(const std::shared_ptr<style>& a_style)
         {
            for (const auto& item : style_names)
               if (std::type_index(typeid(*a_style)) == item.type)
                  return L::t(item.name);
            return L::t(L"Unknown");
         }

         std::shared_ptr<style> make_style(const QString& new_style_name)
         {
            for (const auto& item : style_names)
               if (new_style_name == QString::fromWCharArray(L::t(item.name)))
                  if (item.maker)
                     return item.maker();
            return nullptr;
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order layers.

      class layers_selector_ui
      {
      public:
         layers_selector_ui(layers_selector& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
         : editor(parent)
         {
            build_ui(parent, copy_icon, add_icon, remove_icon, move_up_icon, move_down_icon);
         }

         const layers& get_edited() const
         {
            return edited;
         }

         void set_edited(const layers& ed)
         {
            if (ed == edited)
               return;

            edited = ed;

            std::vector<int> selected = get_selected_indexes();
            if (selected.size() == 0 && ed.size() == 1)
               selected.emplace_back(0);

            fill_ui(selected);
         }

         layers get_selected_layers() const
         {
            std::vector<std::shared_ptr<layer>> selected;
            for (int index : get_selected_indexes())
            {
               selected.emplace_back(edited[index]);
            }
            return selected;
         };

         std::vector<std::shared_ptr<style>> get_selected_styles() const
         {
            std::vector<std::shared_ptr<style>> selected;
            for (auto layer : get_selected_layers())
            {
               if (auto style = extract_style(layer))
               {
                  selected.emplace_back(style);
               }
            }
            return selected;
         };

         void update_list_content()
         {
            disable_feedback++;
            layer_list->blockSignals(disable_feedback > 0);

            int row = 0;
            for (auto& layer : edited)
            {
               if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
               {
                  const auto state = mo_layer->hide ? Qt::CheckState::Unchecked : Qt::CheckState::Checked;
                  auto draw_item = layer_list->item(row, draw_column);
                  draw_item->setCheckState(state);

                  // Note: make icon larger than what was set in the table view
                  //       so that it gets scaled down with some smoothing.
                  const QIcon qicon = get_icon(mo_layer, 128, 64);
                  const QString tiling_name = QString::fromWCharArray(mo_layer->mosaic->tiling.name.c_str());
                  auto tiling_item = layer_list->item(row, tiling_column);
                  tiling_item->setIcon(qicon);
                  tiling_item->setText(tiling_name);

                  const QString style_name = QString::fromWCharArray(get_style_name(mo_layer->style));
                  auto style_item = layer_list->item(row, style_column);
                  style_item->setText(style_name);

                  row++;
               }
            }

            disable_feedback--;
            layer_list->blockSignals(disable_feedback > 0);
         }

      private:
         static std::shared_ptr<style> extract_style(const std::shared_ptr<layer>& layer)
         {
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
            {
               return mo_layer->style;
            }
            else
            {
               return nullptr;
            }
         }

         static std::unique_ptr<QPushButton> make_button(int icon, const wchar_t* tooltip)
         {
            std::unique_ptr<QPushButton> button = std::make_unique<QPushButton>();
            button->setIcon(QIcon(create_pixmap_from_resource(icon)));
            button->setToolTip(QString::fromWCharArray(tooltip));
            return std::move(button);
         }

         void build_ui(layers_selector& parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
         {
            QVBoxLayout* layout = new QVBoxLayout(&parent);
            layout->setContentsMargins(0, 0, 0, 0);

            QWidget* button_panel = new QWidget(&parent);
               QGridLayout* button_layout = new QGridLayout(button_panel);
               button_layout->setContentsMargins(0, 0, 0, 0);
               clone_layer_button = make_button(copy_icon, L::t(L"Copy"));
               button_layout->addWidget(clone_layer_button.get(), 0, 0);
               add_layer_button = make_button(add_icon, L::t(L"Add"));
               button_layout->addWidget(add_layer_button.get(), 0, 1);
               remove_layers_button = make_button(remove_icon, L::t(L"Remove"));
               button_layout->addWidget(remove_layers_button.get(), 0, 2);
               move_layers_up_button = make_button(move_up_icon, L::t(L"Move Up"));
               button_layout->addWidget(move_layers_up_button.get(), 0, 3);
               move_layers_down_button = make_button(move_down_icon, L::t(L"Move Down"));
               button_layout->addWidget(move_layers_down_button.get(), 0, 4);
            layout->addWidget(button_panel);

            QStringList combo_items;
            for (const auto& item : style_names)
               combo_items.append(QString::fromWCharArray(L::t(item.name)));

            layer_list = std::make_unique<table_widget_with_combo>(style_column, combo_items, &parent);
            layer_list->setIconSize(QSize(64, 32));
            layer_list->setColumnCount(3);
            layer_list->setHorizontalHeaderLabels(QStringList(
               {
                  QString::fromWCharArray(L::t(L"Drawn")),
                  QString::fromWCharArray(L::t(L"Mosaic")),
                  QString::fromWCharArray(L::t(L"Style"))
               }));
            layer_list->setShowGrid(false);
            layer_list->horizontalHeader()->setSectionResizeMode(tiling_column, QHeaderView::ResizeMode::Stretch);
            layout->addWidget(layer_list.get());

            layer_list->setEnabled(false);
            clone_layer_button->setEnabled(false);
            add_layer_button->setEnabled(true);
            remove_layers_button->setEnabled(false);
            move_layers_up_button->setEnabled(false);
            move_layers_down_button->setEnabled(false);

            layer_list->connect(layer_list.get(), &QTableWidget::itemSelectionChanged, [&]() { update_selection(); });
            layer_list->connect(layer_list.get(), &QTableWidget::itemChanged, [&](QTableWidgetItem * item) { update_layer(item); });

            clone_layer_button->connect(clone_layer_button.get(), &QPushButton::clicked, [&]() { clone_layer(); });
            add_layer_button->connect(add_layer_button.get(), &QPushButton::clicked, [&]() { add_layer(); });
            remove_layers_button->connect(remove_layers_button.get(), &QPushButton::clicked, [&]() { remove_layers(); });
            move_layers_up_button->connect(move_layers_up_button.get(), &QPushButton::clicked, [&]() { move_layers_up(); });
            move_layers_down_button->connect(move_layers_down_button.get(), &QPushButton::clicked, [&]() { move_layers_down(); });
         }

         void fill_ui(const std::vector<int>& selected)
         {
            disable_feedback++;
            layer_list->blockSignals(disable_feedback > 0);

            layer_list->setRowCount(0);

            for (auto& layer : edited)
            {
               if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
               {
                  const int row = layer_list->rowCount();
                  layer_list->setRowCount(row + 1);

                  const auto state = mo_layer->hide ? Qt::CheckState::Unchecked : Qt::CheckState::Checked;
                  auto draw_item = new QTableWidgetItem();
                  draw_item->setCheckState(state);
                  layer_list->setItem(row, draw_column, draw_item);

                  // Note: make the icon larger than what was set in the table view
                  //       so that it gets scaled down with some smoothing.
                  const QIcon qicon = get_icon(mo_layer, 128, 64);
                  const QString tiling_name = QString::fromWCharArray(mo_layer->mosaic->tiling.name.c_str());
                  auto tiling_item = new QTableWidgetItem(qicon, tiling_name);
                  tiling_item->setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
                  layer_list->setItem(row, tiling_column, tiling_item);

                  const QString style_name = QString::fromWCharArray(get_style_name(mo_layer->style));
                  auto style_item = new QTableWidgetItem(style_name);
                  layer_list->setItem(row, style_column, style_item);
               }
            }

            layer_list->resizeColumnsToContents();
            layer_list->horizontalHeader()->setSectionResizeMode(tiling_column, QHeaderView::ResizeMode::Stretch);

            set_selected_indexes(selected);

            update_enabled();

            disable_feedback--;
            layer_list->blockSignals(disable_feedback > 0);
         }

         void update_enabled()
         {
            auto selected = get_selected_indexes();

            layer_list->setEnabled(edited.size() > 0);
            clone_layer_button->setEnabled(selected.size() > 0);
            add_layer_button->setEnabled(true);
            remove_layers_button->setEnabled(selected.size() > 0);
            move_layers_up_button->setEnabled(edited.size() > 1 && selected.size() > 0);
            move_layers_down_button->setEnabled(edited.size() > 1 && selected.size() > 0);
         }

         void update_selection()
         {
            update_enabled();
            if (editor.selection_changed)
               editor.selection_changed(edited);
         }

         void update_layer(QTableWidgetItem * item)
         {
            if (!item)
               return;

            switch (item->column())
            {
               case draw_column:    return update_drawn(item);
               case style_column:   return update_style(item);
            }
         }

         void update_drawn(QTableWidgetItem * item)
         {
            if (!item)
               return;

            const int row = item->row();
            if (row < 0 || row >= edited.size())
               return;

            edited[row]->hide = (item->checkState() == Qt::CheckState::Unchecked);

            update_layers();
         }

         void update_style(QTableWidgetItem * item)
         {
            if (!item)
               return;

            const int row = item->row();
            if (row < 0 || row >= edited.size())
               return;

            const auto old_style = extract_style(edited[row]);
            auto new_style = make_style(item->text());
            if (new_style)
            {
               if (old_style)
                  new_style->make_similar(*old_style);
               if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(edited[row]))
                  mo_layer->style = new_style;
               else
                  edited[row] = new_style;
            }
            update_layers();
         }

         void update_layers()
         {
            if (edited.size() <= 0)
               return;

            // Note: used to avoid re-calculating the layer when just setting its value in the UI.
            if (disable_feedback)
               return;

            if (editor.layers_changed)
               editor.layers_changed(get_selected_layers());
         }

         void set_selected_indexes(const std::vector<int>& indexes)
         {
            layer_list->clearSelection();
            for (const int row : indexes)
            {
               for (int col = 0; col < layer_list->columnCount(); ++col)
               {
                  const auto item = layer_list->item(row, col);
                  item->setSelected(true);
               }
            }
         }

         std::vector<int> get_selected_indexes() const
         {
            std::vector<int> selected;
            for (int row = 0; row < layer_list->rowCount() && row < edited.size(); ++row)
            {
               for (int col = 0; col < layer_list->columnCount(); ++col)
               {
                  const auto item = layer_list->item(row, col);
                  if (item->isSelected())
                  {
                     selected.emplace_back(row);
                     break;
                  }
               }
            }
            return selected;
         };

         void clone_layer()
         {
            // Note: clone in reverse index order to avoid changing indexes before processing them.
            auto selected = get_selected_indexes();
            std::reverse(selected.begin(), selected.end());
            for (int index : selected)
            {
               edited.emplace(edited.begin() + index, edited[index]->clone());
            }
            fill_ui({});
            update_selection();
         }

         void add_layer()
         {
            if (editor.new_layer_requested)
               editor.new_layer_requested();
         }

         void remove_layers()
         {
            // Note: clone in reverse index order to avoid changing indexes before processing them.
            auto selected = get_selected_indexes();
            std::reverse(selected.begin(), selected.end());
            for (int index : selected)
            {
               edited.erase(edited.begin() + index);
            }
            fill_ui({});
            update_selection();
         }

         void move_layers_up()
         {
            // Treat each target index in order: find if it must receive the layer from below moving up.
            auto selected = get_selected_indexes();
            int target = 0;
            for (int& index : selected)
            {
               while (target < index-1)
                  target++;
               if (target < index)
               {
                  std::swap(edited[target], edited[index]);
                  index--;
               }
               target++;
            }
            fill_ui(selected);
            update_selection();
         }

         void move_layers_down()
         {
            // Treat each target index in reverse order: find if it must receive the layer from up moving down.
            auto selected = get_selected_indexes();
            std::reverse(selected.begin(), selected.end());
            int target = int(edited.size()) - 1;
            for (int& index : selected)
            {
               while (target > index + 1)
                  target--;
               if (target > index)
               {
                  std::swap(edited[target], edited[index]);
                  index++;
               }
               target--;
            }
            fill_ui(selected);
            update_selection();
         }

         static constexpr int draw_column = 0;
         static constexpr int tiling_column = 1;
         static constexpr int style_column = 2;

         layers_selector& editor;
         layers edited;

         std::unique_ptr<table_widget_with_combo> layer_list;
         std::unique_ptr<QPushButton> clone_layer_button;
         std::unique_ptr<QPushButton> add_layer_button;
         std::unique_ptr<QPushButton> remove_layers_button;
         std::unique_ptr<QPushButton> move_layers_up_button;
         std::unique_ptr<QPushButton> move_layers_down_button;

         int disable_feedback = 0;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order layers.

      layers_selector::layers_selector(QWidget* parent, int copy_icon, int add_icon, int remove_icon, int move_up_icon, int move_down_icon)
      : QWidget(parent), ui(std::make_unique<layers_selector_ui>(*this, copy_icon, add_icon, remove_icon, move_up_icon, move_down_icon))
      {
      }

      void layers_selector::set_edited(const layers& edited)
      {
         if (!ui)
            return;

         ui->set_edited(edited);
      }

      const layers& layers_selector::get_edited() const
      {
         static const layers empty;
         if (!ui)
            return empty;

         return ui->get_edited();
      }

      void layers_selector::update_list_content()
      {
         if (!ui)
            return;

         return ui->update_list_content();
      }


      layers layers_selector::get_selected_layers() const
      {
         if (!ui)
            return {};

         return ui->get_selected_layers();
      }

      layers_selector::styles layers_selector::get_selected_styles() const
      {
         if (!ui)
            return {};

         return ui->get_selected_styles();
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
