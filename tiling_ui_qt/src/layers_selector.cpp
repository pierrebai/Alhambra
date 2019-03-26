#include <dak/tiling_ui_qt/layers_selector.h>
#include <dak/tiling_ui_qt/utility.h>

#include <dak/ui_qt/convert.h>

#include <dak/tiling_style/plain.h>
#include <dak/tiling_style/thick.h>
#include <dak/tiling_style/sketch.h>
#include <dak/tiling_style/outline.h>
#include <dak/tiling_style/emboss.h>
#include <dak/tiling_style/filled.h>
#include <dak/tiling_style/interlace.h>

#include <dak/geometry/utility.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgridlayout.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qpushbutton.h>

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

      using geometry::L;
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
            { typeid(int), L"Mixed", nullptr },
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

         std::shared_ptr<style> make_style(const std::wstring& new_style_name)
         {
            for (const auto& item : style_names)
               if (new_style_name == L::t(item.name))
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
         layers_selector_ui(layers_selector& parent, const layers& ed)
         : editor(parent)
         {
            build_ui(parent);
            set_edited(ed);
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
            fill_ui(get_selected_indexes());
         }

         void update_list_content()
         {
            int row = 0;
            for (auto& layer : edited)
            {
               if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
               {
                  std::wstring text = mo_layer->mosaic->tiling.name + std::wstring(L"\n") + mo_layer->style->describe();
                  QString qtext = QString::fromStdWString(text);
                  // Note: make icon larger so that it gets scaled down with some smoothing.
                  QIcon qicon = get_icon(mo_layer, 128, 64);
                  auto item = layer_list->item(row);
                  item->setText(qtext);
                  item->setIcon(qicon);
                  row++;
               }
            }
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

         const wchar_t* get_common_style_name() const
         {
            auto selected = get_selected_styles();
            if (selected.size() <= 0)
               return nullptr;

            if (selected.size() == 1)
               return get_style_name(selected[0]);

            const wchar_t* first_name = get_style_name(selected[0]);
            for (const auto& style : selected)
            {
               if (first_name != get_style_name(style))
               {
                  return L::t(L"Mixed");
               }
            }
            return first_name;
         }

         void build_ui(layers_selector& parent)
         {
            QVBoxLayout* layout = new QVBoxLayout(&parent);
            layout->setContentsMargins(0, 0, 0, 0);

            QWidget* button_panel = new QWidget(&parent);
               QGridLayout* button_layout = new QGridLayout(button_panel);
               button_layout->setContentsMargins(0, 0, 0, 0);
               clone_layer_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Copy")), button_panel);
               button_layout->addWidget(clone_layer_button.get(), 0, 0, 1, 2);
               add_layer_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Add")), button_panel);
               button_layout->addWidget(add_layer_button.get(), 0, 2, 1, 2);
               remove_layers_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Remove")), button_panel);
               button_layout->addWidget(remove_layers_button.get(), 0, 4, 1, 2);
               move_layers_up_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Move Up")), button_panel);
               button_layout->addWidget(move_layers_up_button.get(), 1, 0, 1, 3);
               move_layers_down_button = std::make_unique<QPushButton>(QString::fromWCharArray(L::t(L"Move Down")), button_panel);
               button_layout->addWidget(move_layers_down_button.get(), 1, 3, 1, 3);
            layout->addWidget(button_panel);

            layer_list = std::make_unique<QListWidget>(&parent);
            layer_list->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
            layer_list->setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
            layer_list->setIconSize(QSize(64, 32));
            layout->addWidget(layer_list.get());

            style_editor = std::make_unique<QComboBox>(&parent);
            for (const auto name : style_names)
               style_editor->addItem(QString::fromWCharArray(L::t(name.name)));
            layout->addWidget(style_editor.get());

            layer_list->setEnabled(false);
            clone_layer_button->setEnabled(false);
            add_layer_button->setEnabled(true);
            remove_layers_button->setEnabled(false);
            move_layers_up_button->setEnabled(false);
            move_layers_down_button->setEnabled(false);
            style_editor->setEnabled(false);

            layer_list->connect(layer_list.get(), &QListWidget::itemSelectionChanged, [&]() { update_selection(); });
            clone_layer_button->connect(clone_layer_button.get(), &QPushButton::clicked, [&]() { clone_layer(); });
            add_layer_button->connect(add_layer_button.get(), &QPushButton::clicked, [&]() { add_layer(); });
            remove_layers_button->connect(remove_layers_button.get(), &QPushButton::clicked, [&]() { remove_layers(); });
            move_layers_up_button->connect(move_layers_up_button.get(), &QPushButton::clicked, [&]() { move_layers_up(); });
            move_layers_down_button->connect(move_layers_down_button.get(), &QPushButton::clicked, [&]() { move_layers_down(); });
            style_editor->connect(style_editor.get(), &QComboBox::currentTextChanged, [&](const QString& text) { update_style(text); });
         }

         void fill_ui(const std::vector<int>& selected)
         {
            disable_feedback = true;

            layer_list->clear();

            for (auto& layer : edited)
            {
               if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
               {
                  std::wstring text = mo_layer->mosaic->tiling.name + std::wstring(L"\n") + mo_layer->style->describe();
                  QString qtext = QString::fromStdWString(text);
                  // Note: make icon larger so that it gets scaled down with some smoothing.
                  QIcon qicon = get_icon(mo_layer, 128, 64);
                  auto item = new QListWidgetItem(qicon, qtext);
                  layer_list->addItem(item);
               }
            }

            fill_style_editor();

            set_selected_indexes(selected);

            update_enabled();

            disable_feedback = false;
         }

         void fill_style_editor()
         {
            if (const wchar_t* name = get_common_style_name())
            {
               const QString new_name = QString::fromWCharArray(name);
               if (style_editor->currentText() != new_name)
               {
                  style_editor->setCurrentText(new_name);
               }
            }
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
            style_editor->setEnabled(get_common_style_name() != nullptr);
         }

         void update_selection()
         {
            disable_feedback = true;
            fill_style_editor();
            disable_feedback = false;
            update_enabled();

            if (editor.selection_changed)
               editor.selection_changed(edited);
         }

         void update_style(const QString& new_style_name)
         {
            if (disable_feedback)
               return;

            auto selected = get_selected_indexes();
            for (int index : selected)
            {
               const auto old_style = extract_style(edited[index]);
               auto new_style = make_style(new_style_name.toStdWString());
               if (new_style)
               {
                  if (old_style)
                     new_style->make_similar(*old_style);
                  if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(edited[index]))
                     mo_layer->style = new_style;
                  else
                     edited[index] = new_style;
               }
            }
            fill_ui(selected);
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
               const auto item = layer_list->item(row);
               item->setSelected(true);
            }
         }

         std::vector<int> get_selected_indexes() const
         {
            std::vector<int> selected;
            for (int row = 0; row < layer_list->count() && row < edited.size(); ++row)
            {
               const auto item = layer_list->item(row);
               if (item->isSelected())
               {
                  selected.emplace_back(row);
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

         layers_selector& editor;
         layers edited;

         std::unique_ptr<QListWidget> layer_list;
         std::unique_ptr<QPushButton> clone_layer_button;
         std::unique_ptr<QPushButton> add_layer_button;
         std::unique_ptr<QPushButton> remove_layers_button;
         std::unique_ptr<QPushButton> move_layers_up_button;
         std::unique_ptr<QPushButton> move_layers_down_button;
         std::unique_ptr<QComboBox> style_editor;

         bool disable_feedback = false;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget to select and order layers.

      layers_selector::layers_selector(QWidget* parent)
      : layers_selector(parent, {}, nullptr)
      {
      }

      layers_selector::layers_selector(QWidget* parent, selection_changed_callback sc)
      : layers_selector(parent, {}, sc)
      {
      }

      layers_selector::layers_selector(QWidget* parent, const layers& edited, selection_changed_callback sc)
      : QWidget(parent), ui(std::make_unique<layers_selector_ui>(*this, edited)), selection_changed(sc)
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
