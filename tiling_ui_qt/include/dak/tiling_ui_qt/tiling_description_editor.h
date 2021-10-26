#pragma once

#ifndef DAK_TILING_UI_QT_TILING_DESCRIPTION_EDITOR_H
#define DAK_TILING_UI_QT_TILING_DESCRIPTION_EDITOR_H

#include <dak/tiling/tiling.h>

#include <dak/ui/color.h>
#include <dak/ui/mouse.h>
#include <dak/ui/drawing.h>
#include <dak/ui/qt/message_reporter.h>

#include <QtWidgets/qwidget.h>
#include <QtWidgets/qaction.h>

#include <memory>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::tiling::tiling_t;

      class tiling_description_editor_ui_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // It's used to edit the author and description of the tilings.

      class tiling_description_editor_t : public QWidget
      {
      public:
         // Creation.
         tiling_description_editor_t(QWidget* parent);

         // Tiling management.
         void set_tiling(const std::shared_ptr<tiling_t>& tiling);

         std::wstring get_name() const;
         std::wstring get_description() const;
         std::wstring get_author() const;

      private:
         void build_ui();

         std::shared_ptr<tiling_description_editor_ui_t> my_ui;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

