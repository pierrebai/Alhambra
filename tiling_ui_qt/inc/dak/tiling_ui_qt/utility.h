#pragma once

#ifndef DAK_TILING_UI_QT_UTILITY_H
#define DAK_TILING_UI_QT_UTILITY_H

#include <dak/tiling_style/styled_mosaic.h>

#include <dak/ui/color.h>

#include <memory>

#include <QtGui/qicon.h>

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
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
