#include <dak/tiling_ui_qt/ask.h>

#include <dak/tiling/tiling.h>
#include <dak/tiling/known_tilings.h>
#include <dak/tiling/tiling_io.h>
#include <dak/tiling_style/mosaic_io.h>

#include <dak/utility/text.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <QtWidgets/qerrormessage.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qdir.h>

#include <fstream>

namespace dak
{
   namespace tiling_ui_qt
   {
      using dak::utility::L;

      namespace
      {
         const wchar_t* tiling_file_types = L"Tiling Files (*.tiling)";
         const wchar_t* layered_mosaic_file_types = L"Mosaic Files (*.tap.txt)";
      }


      std::filesystem::path get_user_tilings_folder()
      {
         QDir documentFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
         return documentFolder.absoluteFilePath("Alhambra/tilings").toStdWString();
      }

      std::filesystem::path get_user_mosaics_folder()
      {
         QDir documentFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
         return documentFolder.absoluteFilePath("mosaics").toStdWString();
      }

      std::filesystem::path get_user_tilings_old_folder()
      {
         QDir documentFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
         return documentFolder.absoluteFilePath("tilings").toStdWString();
      }

      std::filesystem::path get_user_mosaics_old_folder()
      {
         QDir documentFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
         return documentFolder.absoluteFilePath("Alhambra/mosaics").toStdWString();
      }

      std::filesystem::path ask_open(const wchar_t* title, const wchar_t* file_types, QWidget* parent, const wchar_t* /*initial_path*/ )
      {
         return QtAdditions::AskOpen(QString::fromWCharArray(title), QString::fromWCharArray(file_types), parent);
      }

      std::filesystem::path ask_save(const wchar_t* title, const wchar_t* file_types, QWidget* parent, const wchar_t* initial_path)
      {
         return QtAdditions::AskSave(QString::fromWCharArray(title), QString::fromWCharArray(file_types), QString::fromWCharArray(initial_path), parent);
      }

      // Show a dialog to open or save a tiling.
      std::shared_ptr<tiling_t> ask_open_tiling(std::filesystem::path& path, QWidget* parent)
      {
         path = ask_open(L::t(L"Load Tiling"), L::t(tiling_file_types), parent, get_user_tilings_folder().c_str());
         if (path.empty())
            return {};
         std::wifstream file(path);
         return dak::tiling::read_tiling(file);
      }

      bool ask_save_tiling(const std::shared_ptr<tiling_t>& tiling, std::filesystem::path& path, QWidget* parent)
      {
         if (!tiling)
            return false;

         path = ask_save(L::t(L"Save Tiling"), L::t(tiling_file_types), parent, get_user_tilings_folder().c_str());
         if (path.empty())
            return false;

         if( tiling->name.empty() )
            tiling->name = path.stem();

         {
            std::wofstream file(path, std::ios::out | std::ios::trunc);
            dak::tiling::write_tiling(tiling, file);
         }

         return true;
      }

      // Show a dialog to open or save a layered mosaic.
      std::vector<std::shared_ptr<styled_mosaic_t>> ask_open_layered_mosaic(const known_tilings_t& knowns, std::filesystem::path& path, QWidget* parent)
      {
         // TODO: make try/catch optional.
         path = ask_open(L::t(L"Load Mosaic"), L::t(layered_mosaic_file_types), parent, get_user_mosaics_folder().c_str());
         if (path.empty())
            return {};
         try
         {
            std::wifstream file(path);
            return tiling_style::read_layered_mosaic(file, knowns);
         }
         catch (std::exception& ex)
         {
            QErrorMessage error(parent);
            error.showMessage(ex.what());
            return {};
         }
      }

      bool ask_save_layered_mosaic(const std::vector<std::shared_ptr<styled_mosaic_t>>& layers, std::filesystem::path& path, QWidget* parent)
      {
         // TODO: make try/catch optional.
         path = ask_save(L::t(L"Save Mosaic"), L::t(layered_mosaic_file_types), parent, get_user_mosaics_folder().c_str());
         if (path.empty())
            return false;
         try
         {
            std::wofstream file(path, std::ios::out | std::ios::trunc);
            tiling_style::write_layered_mosaic(file, layers);
            return true;
         }
         catch (std::exception& ex)
         {
            QErrorMessage error(parent);
            error.showMessage(ex.what());
            return false;
         }
      }

      // Ask yes/no/cancel.
      yes_no_cancel_t ask_yes_no_cancel(const std::wstring& title, const std::wstring& text, QWidget* parent)
      {
         return ask_yes_no_cancel(title.c_str(), text.c_str(), parent);
      }

      yes_no_cancel_t ask_yes_no_cancel(const wchar_t* title, const wchar_t* text, QWidget* parent)
      {
         switch (QtAdditions::AskYesNoCancel(QString::fromWCharArray(title), QString::fromWCharArray(text), parent))
         {
            case QtAdditions::YesNoCancel::No:
               return yes_no_cancel_t::no;

            case QtAdditions::YesNoCancel::Yes:
               return yes_no_cancel_t::yes;

            default:
            case QtAdditions::YesNoCancel::Cancel:
               return yes_no_cancel_t::cancel;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
