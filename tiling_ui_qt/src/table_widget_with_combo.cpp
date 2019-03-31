#include <dak/tiling_ui_qt/table_widget_with_combo.h>

#include <QtCore/qtimer.h>

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qheaderview.h>
#include <QtWidgets/qstyle.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      // Create the delegate with the combo-box items list.
      item_delegate_with_combo::item_delegate_with_combo(const QStringList& items, QObject *parent)
      : QStyledItemDelegate(parent), combo_items(items)
      {
      }

      // Create the combo-box and populate it.
      QWidget *item_delegate_with_combo::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
      {
         QComboBox *cb = new QComboBox(parent);
         const int row = index.row();
         for (const auto text : combo_items)
            cb->addItem(text);
         cb->connect(cb, &QComboBox::currentTextChanged, [self = this, cb = cb, index = index]()
         {
            if (cb->isVisible())
            {
               const_cast<item_delegate_with_combo *>(self)->commitData(cb);
               const_cast<item_delegate_with_combo *>(self)->closeEditor(cb);
            }
         });
         return cb;
      }

      // Get the index of the text in the combo-box that matches the current value of the item.
      // If it is valid, adjust the combo-box.
      void item_delegate_with_combo::setEditorData(QWidget *editor, const QModelIndex &index) const
      {
         QComboBox *cb = qobject_cast<QComboBox *>(editor);
         if (!cb)
            return;

         const QString currentText = index.data(Qt::EditRole).toString();
         const int cbIndex = cb->findText(currentText);
         if (cbIndex >= 0)
            cb->setCurrentIndex(cbIndex);
         cb->showPopup();
      }

      // Save combo-box data in the view item.
      void item_delegate_with_combo::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
      {
         QComboBox *cb = qobject_cast<QComboBox *>(editor);
         if (!cb)
            return;

         model->setData(index, cb->currentText(), Qt::EditRole);
      }

      void item_delegate_with_combo::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
      {
         QStyleOptionComboBox box;
         box.state = option.state;

         box.rect = option.rect;
         box.currentText = index.data(Qt::EditRole).toString();

         QApplication::style()->drawComplexControl(QStyle::CC_ComboBox, &box, painter, 0);
         QApplication::style()->drawControl(QStyle::CE_ComboBoxLabel, &box, painter, 0);
      }

      QSize item_delegate_with_combo::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
      {
         QStyleOptionComboBox box;
         box.state = option.state;

         box.rect = option.rect;

         QSize size(0, 0);
         for (const auto text : combo_items)
         {
            box.currentText = text;
            QRect rect = QApplication::style()->itemTextRect(option.fontMetrics, option.rect, Qt::AlignCenter, true, text);
            size = size.expandedTo(QApplication::style()->sizeFromContents(QStyle::ContentsType::CT_ComboBox, &box, rect.size(), option.widget));
         }
         return size;
      }

      // Create the table widget with the combo-box items list.
      table_widget_with_combo::table_widget_with_combo(int col, const QStringList& items, QWidget* parent)
         : QTableWidget(parent), combo_column(col), combo_items(items)
      {
         setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
         setSelectionMode(QAbstractItemView::SelectionMode::ExtendedSelection);
         setEditTriggers(QAbstractItemView::EditTrigger::EditKeyPressed | QAbstractItemView::EditTrigger::SelectedClicked | QAbstractItemView::EditTrigger::AnyKeyPressed);
         verticalHeader()->hide();
         setColumnCount(col + 1);
         setItemDelegateForColumn(1, new item_delegate_with_combo(items, this));
      }

      // Process mouse events to create the combo-box on the first mouse click.
      void table_widget_with_combo::mousePressEvent(QMouseEvent *event)
      {
         if (event->button() == Qt::LeftButton)
         {
            const QModelIndex index = indexAt(event->pos());
            if (index.column() == combo_column)
            {
               for (int row = 0; row < rowCount(); ++row)
                  if (auto other_item = item(row, combo_column > 0 ? 0 : 1))
                     other_item->setSelected(row == index.row());
               QTimer::singleShot(1, [self=this,index=index]()
               {
                  self->edit(index);
               });
               event->setAccepted(true);
               return;
            }
         }
         QTableWidget::mousePressEvent(event);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
