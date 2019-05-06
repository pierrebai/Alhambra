#pragma once

#ifndef DAK_UI_QT_UTILITY_H
#define DAK_UI_QT_UTILITY_H

#include <dak/tiling_style/styled_mosaic.h>

#include <dak/ui/color.h>

#include <memory>
#include <functional>

#include <QtGui/qicon.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qbitmap.h>

#include <QtWidgets/qaction.h>
#include <QtWidgets/qtoolbutton.h>

namespace dak
{
   namespace ui_qt
   {
      using dak::tiling_style::styled_mosaic;
      using dak::tiling::mosaic;
      using dak::ui::color;

      // Create an icon for the given mosaic.
      QIcon get_icon(const std::shared_ptr<styled_mosaic>& sm, int w, int h);
      QIcon get_icon(const std::shared_ptr<mosaic>& mosaic, const color& co, int w, int h);

      // Create toolbar buttons.
      // Note: icon is a bitmap resource ID.
      QToolButton* create_tool_button(const wchar_t* text, int icon = 0, QKeySequence key = QKeySequence(), const wchar_t* tooltip = nullptr, std::function<void()> = nullptr);
      QToolButton* create_tool_button(QAction* action);
      QToolButton* create_tool_button();

      // Create action.
      // Note: icon is a bitmap resource ID.
      QAction* create_action(const wchar_t* text, int icon = 0, QKeySequence key = QKeySequence(), const wchar_t* tooltip = nullptr, std::function<void()> = nullptr);

      // Create pixmap from a bitmap resource. A mask will automatically be created.
      QPixmap create_pixmap_from_resource(int res);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
