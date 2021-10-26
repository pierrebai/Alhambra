#include <dak/tiling_ui_qt/tiling_description_editor.h>

#include <dak/utility/text.h>

#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qtextedit.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qlabel.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using namespace dak::ui;
      using namespace dak::ui::qt;
      using namespace dak::tiling;

      using dak::utility::L;

      ////////////////////////////////////////////////////////////////////////////
      //
      // UI implementation of the tiling editor.

      class tiling_description_editor_ui_t : public QWidget
      {
      public:
         tiling_description_editor_ui_t(QWidget* parent);

         // Tiling management.
         void set_tiling(const std::shared_ptr<tiling_t>& tiling);

         std::wstring get_name() const;
         std::wstring get_description() const;
         std::wstring get_author() const;

      private:
         void build_ui();
         void fill_ui(const std::shared_ptr<tiling_t>& tiling);

         QLineEdit* my_name_text = nullptr;
         QLineEdit* my_author_text = nullptr;
         QTextEdit* my_description_text = nullptr;
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Tiling editor UI.

      // Creation.
      tiling_description_editor_ui_t::tiling_description_editor_ui_t(QWidget* parent)
      : QWidget(parent)
      {
         build_ui();
         set_tiling(nullptr);
      }

      // Tiling management.

      void tiling_description_editor_ui_t::set_tiling(const std::shared_ptr<tiling_t>& tiling)
      {
         fill_ui(tiling);
      }

      void tiling_description_editor_ui_t::build_ui()
      {
         QVBoxLayout* layout = new QVBoxLayout(this);
            QLabel* name_label = new QLabel(QString::fromWCharArray(L::t(L"Tiling Name")));
            layout->addWidget(name_label);

            my_name_text = new QLineEdit(this);
            layout->addWidget(my_name_text);

            QLabel* author_label = new QLabel(QString::fromWCharArray(L::t(L"Author")));
            layout->addWidget(author_label);

            my_author_text = new QLineEdit(this);
            layout->addWidget(my_author_text);

            QLabel* description_label = new QLabel(QString::fromWCharArray(L::t(L"Description")));
            layout->addWidget(description_label);

            my_description_text = new QTextEdit(this);
            layout->addWidget(my_description_text);

      }

      void tiling_description_editor_ui_t::fill_ui(const std::shared_ptr<tiling_t>& tiling)
      {
         if (!tiling)
            return;

         my_description_text->setText(QString::fromWCharArray(tiling->description.c_str()));
         my_author_text->setText(QString::fromWCharArray(tiling->author.c_str()));
         my_name_text->setText(QString::fromWCharArray(tiling->name.c_str()));
      }

      std::wstring tiling_description_editor_ui_t::get_name() const
      {
         if (!my_name_text)
            return {};

         return my_name_text->text().toStdWString();

      }

      std::wstring tiling_description_editor_ui_t::get_description() const
      {
         if (!my_description_text)
            return {};

         return my_description_text->toPlainText().toStdWString();
      }

      std::wstring tiling_description_editor_ui_t::get_author() const
      {
         if (!my_author_text)
            return {};

         return my_author_text->text().toStdWString();
      }


      ////////////////////////////////////////////////////////////////////////////
      //
      // Tiling editor.

      // Creation.
      tiling_description_editor_t::tiling_description_editor_t(QWidget* parent)
      : QWidget(parent), my_ui(std::shared_ptr<tiling_description_editor_ui_t>(new tiling_description_editor_ui_t(this)))
      {
         build_ui();
      }

      void tiling_description_editor_t::set_tiling(const std::shared_ptr<tiling_t>& tiling)
      {
         my_ui->set_tiling(tiling);
      }

      std::wstring tiling_description_editor_t::get_description() const
      {
         return my_ui->get_description();
      }

      std::wstring tiling_description_editor_t::get_author() const
      {
         return my_ui->get_author();
      }

      std::wstring tiling_description_editor_t::get_name() const
      {
         return my_ui->get_name();
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // UI.

      void tiling_description_editor_t::build_ui()
      {
         auto layout = new QVBoxLayout(this);
         layout->addWidget(my_ui.get());
         layout->setMargin(0);
         setContentsMargins(0, 0, 0, 0);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

