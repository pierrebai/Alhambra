#pragma once

#ifndef DAK_TILING_UI_QT_TABLE_WIDGET_WITH_COMBO_H
#define DAK_TILING_UI_QT_TABLE_WIDGET_WITH_COMBO_H

#include <QtGui/qevent.h>

#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qstyleditemdelegate.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A QTableWidget containing a combo-box that appears
      // immediately on mouse-click.
      //
      // To work we need a custom table widget and a custom item delagate:
      //
      //     - Override mouse-processing in the table to show the
      //       combo-box editor on the first click.
      //
      //     - Pop-up the combo-box content immediately in the delegate
      //       when creating the editor.
      //
      //     - Commit the data and end the editor immediately when
      //       when an item is selected in the popped-up combo-box.
      //
      //     - Use custom paint in the delagate to draw the data
      //       with the look of a combo-box. (So the user knows that
      //       there is a usable combo-box there.)
      //
      //     - Use a custom sizing hint in the delegate to make sure
      //       the item can fit the combo-box and its widest text.
      //

      class table_widget_with_combo : public QTableWidget
      {
      public:
         // The column cotaining a combo-box and the text items.
         int combo_column;
         QStringList combo_items;

         // Create the table widget with the combo-box items list.
         table_widget_with_combo(int col, const QStringList& items, QWidget* parent = nullptr);

      protected:
         // Process mouse events to create the combo-box on the first mouse click.
         void mousePressEvent(QMouseEvent *event) override;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // A QStyledItemDelegate containing a combo-box that appears
      // immediately on mouse-click.
      //
      // To work the custom item delagate must:
      //
      //     - Pop-up the combo-box content immediately in the delegate
      //       when creating the editor.
      //
      //     - Commit the data and end the editor immediately when
      //       when an item is selected in the popped-up combo-box.
      //
      //     - Use custom paint in the delagate to draw the data
      //       with the look of a combo-box. (So the user knows that
      //       there is a usable combo-box there.)
      //
      //     - Use a custom sizing hint in the delegate to make sure
      //       the item can fit the combo-box and its widest text.
      //

      class item_delegate_with_combo : public QStyledItemDelegate
      {
      public:
         QStringList combo_items;

         // Create the delegate with the combo-box items list.
         item_delegate_with_combo(const QStringList& items, QObject *parent = nullptr);
         ~item_delegate_with_combo() { }

         // Create the combo-box and populate it.
         QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

         // Get the index of the text in the combo-box that matches the current value of the item.
         // If it is valid, adjust the combo-box.
         void setEditorData(QWidget *editor, const QModelIndex &index) const override;

         // Save combo-box data in the view item.
         void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

         // Paint the item to look like a combo-box.
         void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

         // Size the item like a combo-box, using its widest text element.
         QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
