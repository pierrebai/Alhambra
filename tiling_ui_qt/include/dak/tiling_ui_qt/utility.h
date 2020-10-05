#pragma once

#ifndef DAK_TILING_UI_QT_UTILITY_H
#define DAK_TILING_UI_QT_UTILITY_H

#include <dak/tiling_style/styled_mosaic.h>

#include <dak/ui/color.h>

#include <memory>

#include <QtGui/qicon.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qbitmap.h>

#include <QtWidgets/qaction.h>
#include <QtWidgets/qtoolbutton.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling_style::styled_mosaic;
      using dak::tiling::mosaic;
      using dak::ui::color;

      // Create an icon for the given mosaic.
      QIcon get_icon(const std::shared_ptr<styled_mosaic>& sm, int w, int h);
      QIcon get_icon(const std::shared_ptr<mosaic>& mosaic, const color& co, int w, int h);

      QToolButton* create_tool_button(const wchar_t* text, int icon);
      QToolButton* create_tool_button(QAction* action);
      QToolButton* create_tool_button();

      QAction* create_action(const wchar_t* text, int icon, QKeySequence key = QKeySequence());

      QPixmap create_pixmap_from_resource(int res);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
