#include <dak/tiling_ui_qt/tiling_window.h>
#include <dak/tiling_ui_qt/tiling_selector.h>

#include <dak/ui_qt/utility.h>
#include <dak/ui_qt/ask.h>

#include <dak/tiling/tiling_io.h>

#include <dak/utility/text.h>

#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qlabel.h>

#include <QtGui/qevent.h>

#include <fstream>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::utility::L;
      using namespace ui_qt;

      // Creation.
      tiling_window::tiling_window(dak::tiling::known_tilings& known_tilings, const tiling_editor_icons& icons, QWidget *parent)
         : QMainWindow(parent), known_tilings(known_tilings)
      {
         /////////////////////////////////////////////////////////////
         // Program window.
         setWindowTitle(L::t("Tiling Designer"));

         build_actions(icons);
         build_ui(icons);
      }

      void tiling_window::build_actions(const tiling_editor_icons& icons)
      {
         new_action = create_tool_button(L::t(L"New Tiling"), icons.tiling_new, 'N', L::t(L"Start a new tiling design. (Shortcut: n)"), [self = this]()
         {
            self->new_tiling();
         });

         open_action = create_tool_button(L::t(L"Open..."), icons.tiling_open, 'O', L::t(L"Open a tiling design. (Shortcut: o)"), [self = this]()
         {
            self->open_tiling();
         });

         select_action = create_tool_button(L::t(L"Select..."), icons.tiling_open, QKeySequence("Shift+O"), L::t(L"Select a tiling design among built-in ones. (Shortcut: <shift> + o)"), [icons, self = this]()
         {
            self->select_tiling(icons);
         });

         save_action = create_tool_button(L::t(L"Save As..."), icons.tiling_save, 'S', L::t(L"Save the tiling design. (Shortcut: s)"), [self = this]()
         {
            self->save_tiling();
         });
      }

      void tiling_window::build_ui(const tiling_editor_icons& icons)
      {
         // Prepare the designer panel itself.
         editor = new tiling_editor(icons, this);

         setCentralWidget(editor);

         // Create a toolbar for end-user actions.
         QToolBar* toolbar = new QToolBar;

         // Add direct actions.
         toolbar->addWidget(new_action);
         toolbar->addWidget(open_action);
         toolbar->addWidget(select_action);
         toolbar->addWidget(save_action);

         toolbar->addSeparator();

         toolbar->addWidget(create_tool_button(editor->add_poly_action));
         toolbar->addWidget(create_tool_button(editor->draw_poly_toggle));
         toolbar->addWidget(create_tool_button(editor->copy_poly_toggle));
         toolbar->addWidget(create_tool_button(editor->delete_poly_toggle));
         toolbar->addWidget(create_tool_button(editor->move_poly_toggle));

         toolbar->addSeparator();

         toolbar->addWidget(create_tool_button(editor->toggle_inclusion_toggle));
         toolbar->addWidget(create_tool_button(editor->exclude_all_action));
         toolbar->addWidget(create_tool_button(editor->fill_trans_action));
         toolbar->addWidget(create_tool_button(editor->remove_excluded_action));

         toolbar->addSeparator();

         toolbar->addWidget(create_tool_button(editor->draw_trans_toggle));
         toolbar->addWidget(create_tool_button(editor->clear_trans_action));

         toolbar->addSeparator();

         toolbar->addWidget(create_tool_button(editor->pan_toggle));
         toolbar->addWidget(create_tool_button(editor->rotate_toggle));
         toolbar->addWidget(create_tool_button(editor->zoom_toggle));

         addToolBar(Qt::ToolBarArea::TopToolBarArea, toolbar);

         addAction(editor->copy_poly_action);
         addAction(editor->delete_poly_action);
         addAction(editor->toggle_inclusion_action);

         // Add panel to edit the name, description and author. TODO
         //tiling_desc = new tiling_description_editor();
         //addDockWidget( tiling_desc );

         setAttribute(Qt::WA_DeleteOnClose);
      }

      void tiling_window::closeEvent(QCloseEvent* ev)
      {
         if (save_if_required(L::t(L"close the window"), L::t(L"closing the window")))
            QWidget::closeEvent(ev);
         else
            ev->ignore();
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Open a design.

      void tiling_window::set_tiling(const tiling& tiling, const file_path& file)
      {
         original_tiling = tiling;
         original_file = file;
         //tiling_desc.set_tiling( tiling );
         editor->set_tiling(tiling);
      }

      void tiling_window::new_tiling()
      {
         if (!save_if_required(L::t(L"start a new tiling"), L::t(L"starting a new tiling")))
            return;

         set_tiling(tiling(), file_path());
         return;
      }

      void tiling_window::select_tiling(const tiling_editor_icons& icons)
      {
         if (!save_if_required(L::t(L"edit another tiling"), L::t(L"editing another tiling")))
            return;

         auto selector = new tiling_selector(known_tilings, icons, this, [self = this](const std::shared_ptr<mosaic>& mo)
         {
            self->set_tiling(mo->tiling, file_path());
         });
         selector->show();
      }

      void tiling_window::open_tiling()
      {
         if (!save_if_required(L::t(L"edit another tiling"), L::t(L"editing another tiling")))
            return;

         try
         {
            std::experimental::filesystem::path path;
            tiling tiling = ask_open_tiling(path, this);
            if (tiling.is_invalid())
               return;
            set_tiling(tiling, path);
         }
         catch (const std::exception& e)
         {
            const std::wstring error_msg = utility::convert(e.what());
            const std::wstring error = error_msg.length() > 0
                                     ? std::wstring(L::t(L"Reason given: ")) + error_msg + std::wstring(L::t(L"."))
                                     : std::wstring(L::t(L"No reason given for the error."));
            editor->report_error(std::wstring(L::t(L"Cannot read the selected tiling!\n")) + error);
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Save the design.

      bool tiling_window::has_original_data_changed()
      {
         // Note: if the current tiling has no tile, we pretend it didn't change
         //       because there is nothing to be lost if closed.
         tiling current_tiling = create_tiling_from_data(original_file);
         return (!current_tiling.tiles.empty() && current_tiling != original_tiling);
      }

      tiling tiling_window::create_tiling_from_data(const file_path& file)
      {
         auto tiling = editor->create_tiling();
         if (tiling.is_invalid())
            return tiling;

         // TODO: tiling description.
         //tiling_desc.fill_tiling_Info(tiling);

         if (tiling.name.length() <= 0)
            tiling.name = file.filename();

         return tiling;
      }

      bool tiling_window::save_if_required(const std::wstring& action, const std::wstring& actioning)
      {
         if (has_original_data_changed())
         {
            if (!editor->verify_tiling(L""))
            {
               yes_no_cancel answer = ask_yes_no_cancel(
                  L::t(L"Unsaved Tiling Warning"),
                  std::wstring(L::t(L"The current tiling is incomplete and will be lost.\nAre you sure you want to ")) + action + L::t(L"?"),
                  this);
               if (answer != yes_no_cancel::yes)
                  return false;
            }
            else
            {
               yes_no_cancel answer = ask_yes_no_cancel(
                  L::t(L"Unsaved Tiling Warning"),
                  std::wstring(L::t(L"The current tiling has not been saved.\nDo you want to save it before ")) + actioning + L::t(L"?"),
                  this);
               if (answer == yes_no_cancel::cancel)
                  return false;
               else if (answer == yes_no_cancel::yes)
                  if (!save_tiling())
                     return false;
            }
         }

         return true;
      }

      bool tiling_window::save_tiling()
      {
         if (!editor->verify_tiling(L::t(L"save")))
            return false;

         try
         {
            file_path path;
            tiling tiling = create_tiling_from_data(path);
            if (!ask_save_tiling(tiling, path, this))
               return false;
            original_file = path;
            original_tiling = tiling;
            if (known_tilings.end() == std::find(known_tilings.begin(), known_tilings.end(), tiling))
               known_tilings.push_back(tiling);
            return true;
         }
         catch (const std::exception& e)
         {
            editor->report_error(std::wstring(L::t(L"Cannot save the tiling!\n")) +
                                 std::wstring(L::t(L"Error: ")) + utility::convert(e.what()));
            return false;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

