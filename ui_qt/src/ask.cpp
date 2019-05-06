#include <dak/ui_qt/ask.h>

#include <dak/tiling/tiling.h>
#include <dak/tiling/known_tilings.h>
#include <dak/tiling/tiling_io.h>
#include <dak/tiling_style/style_io.h>

#include <dak/utility/text.h>

#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qmessagebox.h>

#include <fstream>

namespace dak
{
   namespace ui_qt
   {
      using dak::utility::L;

      namespace
      {
         const wchar_t* tiling_file_types = L"Tiling Files (*.tiling)";
         const wchar_t* layered_mosaic_file_types = L"Mosaic Files (*.tap.txt)";
      }


      std::experimental::filesystem::path ask_open(const wchar_t* title, const wchar_t* file_types, QWidget* parent)
      {
         const QString qfn = QFileDialog::getOpenFileName(
            parent, QString::fromWCharArray(title), QString(), QString::fromWCharArray(file_types));
         const std::wstring s = qfn.toStdWString();
         return s;
      }

      std::experimental::filesystem::path ask_save(const wchar_t* title, const wchar_t* file_types, QWidget* parent)
      {
         const QString qfn = QFileDialog::getSaveFileName(
            parent, QString::fromWCharArray(title), QString(), QString::fromWCharArray(file_types));
         const std::wstring s = qfn.toStdWString();
         return s;
      }

      // Show a dialog to open or save a tiling.
      tiling ask_open_tiling(std::experimental::filesystem::path& path, QWidget* parent)
      {
         path = ask_open(L::t(L"Load Tiling"), L::t(tiling_file_types), parent);
         if (path.empty())
            return {};
         std::wifstream file(path);
         return dak::tiling::read_tiling(file);
      }

      bool ask_save_tiling(const tiling& tiling, std::experimental::filesystem::path& path, QWidget* parent)
      {
         path = ask_save(L::t(L"Save Tiling"), L::t(tiling_file_types), parent);
         if (path.empty())
            return false;
         std::wofstream file(path, std::ios::out | std::ios::trunc);
         if (tiling.name.empty())
         {
            dak::tiling::tiling named_tiling(tiling);
            named_tiling.name = path.stem();
            dak::tiling::write_tiling(named_tiling, file);
         }
         else
         {
            dak::tiling::write_tiling(tiling, file);
         }
         return true;
      }

      // Show a dialog to open or save a layered mosaic.
      std::vector<std::shared_ptr<ui::layer>> ask_open_layered_mosaic(const known_tilings& knowns, std::experimental::filesystem::path& path, QWidget* parent)
      {
         // TODO: make try/catch optional.
         path = ask_open(L::t(L"Load Mosaic"), L::t(layered_mosaic_file_types), parent);
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

      bool ask_save_layered_mosaic(const std::vector<std::shared_ptr<ui::layer>>& layers, std::experimental::filesystem::path& path, QWidget* parent)
      {
         // TODO: make try/catch optional.
         path = ask_save(L::t(L"Save Mosaic"), L::t(layered_mosaic_file_types), parent);
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
      yes_no_cancel ask_yes_no_cancel(const std::wstring& title, const std::wstring& text, QWidget* parent)
      {
         return ask_yes_no_cancel(title.c_str(), text.c_str(), parent);
      }

      yes_no_cancel ask_yes_no_cancel(const wchar_t* title, const wchar_t* text, QWidget* parent)
      {
         QMessageBox box;
         box.setWindowTitle(QString::fromWCharArray(title));
         box.setText(QString::fromWCharArray(text));
         box.setStandardButtons(QMessageBox::StandardButton::Yes | QMessageBox::StandardButton::Cancel | QMessageBox::StandardButton::No );
         box.setDefaultButton(QMessageBox::StandardButton::Cancel);
         switch (box.exec())
         {
            case QMessageBox::StandardButton::Yes:
               return yes_no_cancel::yes;
            case QMessageBox::StandardButton::No:
               return yes_no_cancel::no;
            case QMessageBox::StandardButton::Cancel:
            default:
               return yes_no_cancel::cancel;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
