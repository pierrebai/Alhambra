#include <dak/tiling_ui_qt/tiling_window.h>
#include <dak/tiling_ui_qt/tiling_selector.h>
#include <dak/tiling_ui_qt/ask.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <dak/tiling/tiling_io.h>
#include <dak/tiling/translation_tiling.h>

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
      using namespace ui::qt;
      using namespace QtAdditions;
      using dak::tiling::translation_tiling_t;

      // Creation.
      tiling_window_t::tiling_window_t(dak::tiling::known_tilings_t& known_tilings, const tiling_editor_icons_t& icons, QWidget *parent)
         : QMainWindow(parent), known_tilings(known_tilings)
      {
         /////////////////////////////////////////////////////////////
         // Program window.
         setWindowTitle(L::t("Tiling Designer"));

         build_actions(icons);
         build_ui(icons);
      }

      void tiling_window_t::build_actions(const tiling_editor_icons_t& icons)
      {
         new_action = CreateToolButton(L::t(L"New Tiling"), icons.tiling_new, 'N', L::t(L"Start a new tiling design. (Shortcut: n)"), [self = this]()
         {
            self->new_tiling();
         });

         open_action = CreateToolButton(L::t(L"Open..."), icons.tiling_open, 'O', L::t(L"Open a tiling design. (Shortcut: o)"), [self = this]()
         {
            self->open_tiling();
         });

         select_action = CreateToolButton(L::t(L"Select..."), icons.tiling_open, QKeySequence("Shift+O"), L::t(L"Select a tiling design among built-in ones. (Shortcut: <shift> + o)"), [icons, self = this]()
         {
            self->select_tiling(icons);
         });

         save_action = CreateToolButton(L::t(L"Save As..."), icons.tiling_save, 'S', L::t(L"Save the tiling design. (Shortcut: s)"), [self = this]()
         {
            self->save_tiling();
         });
      }

      void tiling_window_t::build_ui(const tiling_editor_icons_t& icons)
      {
         // Prepare the designer panel itself.
         editor = new tiling_editor_t(icons, this);

         setCentralWidget(editor);

         // Create a toolbar for end-user actions.
         QToolBar* toolbar = new QToolBar;

         // Add direct actions.
         toolbar->addWidget(new_action);
         toolbar->addWidget(open_action);
         toolbar->addWidget(select_action);
         toolbar->addWidget(save_action);

         toolbar->addSeparator();

         toolbar->addWidget(CreateToolButton(editor->add_poly_action));
         toolbar->addWidget(CreateToolButton(editor->draw_poly_toggle));
         toolbar->addWidget(CreateToolButton(editor->copy_poly_toggle));
         toolbar->addWidget(CreateToolButton(editor->delete_poly_toggle));
         toolbar->addWidget(CreateToolButton(editor->move_poly_toggle));

         toolbar->addSeparator();

         toolbar->addWidget(CreateToolButton(editor->toggle_inclusion_toggle));
         toolbar->addWidget(CreateToolButton(editor->exclude_all_action));
         toolbar->addWidget(CreateToolButton(editor->fill_trans_action));
         toolbar->addWidget(CreateToolButton(editor->remove_excluded_action));

         toolbar->addSeparator();

         toolbar->addWidget(CreateToolButton(editor->draw_trans_toggle));
         toolbar->addWidget(CreateToolButton(editor->draw_inflation_toggle));
         toolbar->addWidget(CreateToolButton(editor->clear_trans_action));

         toolbar->addSeparator();

         toolbar->addWidget(CreateToolButton(editor->pan_toggle));
         toolbar->addWidget(CreateToolButton(editor->rotate_toggle));
         toolbar->addWidget(CreateToolButton(editor->zoom_toggle));

         addToolBar(Qt::ToolBarArea::TopToolBarArea, toolbar);

         addAction(editor->copy_poly_action);
         addAction(editor->delete_poly_action);
         addAction(editor->toggle_inclusion_action);

         // Add panel to edit the name, description and author. TODO
         //tiling_desc = new tiling_description_editor();
         //addDockWidget( tiling_desc );

         setAttribute(Qt::WA_DeleteOnClose);
      }

      void tiling_window_t::closeEvent(QCloseEvent* ev)
      {
         if (save_if_required(L::t(L"close the window"), L::t(L"closing the window")))
            QWidget::closeEvent(ev);
         else
            ev->ignore();
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Open a design.

      void tiling_window_t::set_tiling(const std::shared_ptr<tiling_t>& tiling, const file_path_t& file)
      {
         original_tiling = tiling;
         original_file = file;
         //tiling_desc.set_tiling( tiling );
         editor->set_tiling(tiling);
      }

      void tiling_window_t::new_tiling()
      {
         if (!save_if_required(L::t(L"start a new tiling"), L::t(L"starting a new tiling")))
            return;

         set_tiling(std::make_shared<translation_tiling_t>(), file_path_t());
         return;
      }

      void tiling_window_t::select_tiling(const tiling_editor_icons_t& icons)
      {
         if (!save_if_required(L::t(L"edit another tiling"), L::t(L"editing another tiling")))
            return;

         auto selector = new tiling_selector_t(known_tilings, icons, this, [self = this](const std::shared_ptr<tiling_t>& tiling)
         {
            self->set_tiling(tiling, file_path_t());
         });
         selector->show();
      }

      void tiling_window_t::open_tiling()
      {
         if (!save_if_required(L::t(L"edit another tiling"), L::t(L"editing another tiling")))
            return;

         try
         {
            std::filesystem::path path;
            auto tiling = ask_open_tiling(path, this);
            if (!tiling || tiling->is_invalid())
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

      bool tiling_window_t::has_original_data_changed()
      {
         // Note: if the current tiling has no tile, we pretend it didn't change
         //       because there is nothing to be lost if closed.
         auto current_tiling = create_tiling_from_data(original_file);
         return (original_tiling && current_tiling && !current_tiling->tiles.empty() && *current_tiling != *original_tiling);
      }

      std::shared_ptr<tiling_t> tiling_window_t::create_tiling_from_data(const file_path_t& file)
      {
         auto tiling = editor->create_tiling();
         if (!tiling || tiling->is_invalid())
            return tiling;

         // TODO: tiling description.
         tiling->description = L::t(L"(Description)");

         if (tiling->name.length() <= 0)
            tiling->name = file.filename();

         return tiling;
      }

      bool tiling_window_t::save_if_required(const std::wstring& action, const std::wstring& actioning)
      {
         if (has_original_data_changed())
         {
            if (!editor->verify_tiling(L""))
            {
               yes_no_cancel_t answer = ask_yes_no_cancel(
                  L::t(L"Unsaved Tiling Warning"),
                  std::wstring(L::t(L"The current tiling is incomplete and will be lost.\nAre you sure you want to ")) + action + L::t(L"?"),
                  this);
               if (answer != yes_no_cancel_t::yes)
                  return false;
            }
            else
            {
               yes_no_cancel_t answer = ask_yes_no_cancel(
                  L::t(L"Unsaved Tiling Warning"),
                  std::wstring(L::t(L"The current tiling has not been saved.\nDo you want to save it before ")) + actioning + L::t(L"?"),
                  this);
               if (answer == yes_no_cancel_t::cancel)
                  return false;
               else if (answer == yes_no_cancel_t::yes)
                  if (!save_tiling())
                     return false;
            }
         }

         return true;
      }

      bool tiling_window_t::save_tiling()
      {
         if (!editor->verify_tiling(L::t(L"save")))
            return false;

         try
         {
            file_path_t path;
            auto tiling = create_tiling_from_data(path);
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

