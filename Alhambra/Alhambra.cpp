#include <dak/tiling_style/thick.h>

#include <dak/tiling_style/known_mosaics_generator.h>
#include <dak/tiling_style/styled_mosaic.h>
#include <dak/tiling_style/style_io.h>

#include <dak/ui_qt/layered_canvas.h>
#include <dak/ui_qt/convert.h>
#include <dak/ui_qt/drawing.h>

#include <dak/tiling_ui_qt/styles_editor.h>
#include <dak/tiling_ui_qt/figure_editor.h>
#include <dak/tiling_ui_qt/layers_selector.h>
#include <dak/tiling_ui_qt/tiling_selector.h>
#include <dak/tiling_ui_qt/utility.h>

#include <dak/geometry/utility.h>

#include <QtWidgets/qapplication>
#include <QtWidgets/qapplication>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qlistwidget.h>
#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qdockwidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWidgets/qtoolbutton.h>
#include <QtWidgets/qerrormessage.h>

#include <QtGui/qpainter.h>

#include <QtWinExtras/qwinfunctions.h>

#include <QtSvg/qsvggenerator.h>

#include <fstream>

#include "resource.h"

using namespace dak::geometry;
using namespace dak::tiling;
using namespace dak::tiling_style;
using namespace dak::tiling_style;
using namespace dak::ui_qt;
using namespace dak::tiling_ui_qt;

static HINSTANCE appInstance;

int main(int argc, char **argv)
{
   QScopedPointer<QApplication> app(new QApplication(argc, argv));

   L::initialize();

   /////////////////////////////////////////////////////////////////////////
   //
   // The window UI contents.

   QToolBar* toolbar = new QToolBar();
      QToolButton* previous_mosaic_button = create_tool_button(L::t(L"Previous Mosaic"), IDB_MOSAIC_PREVIOUS);
      toolbar->addWidget(previous_mosaic_button);

      QToolButton* next_mosaic_button = create_tool_button(L::t(L"Next Mosaic"), IDB_MOSAIC_NEXT);
      next_mosaic_button->setIconSize(QSize(48, 48));
      toolbar->addWidget(next_mosaic_button);

      toolbar->addSeparator();

      QToolButton* load_mosaic_button = create_tool_button(L::t(L"Load Mosaic"), IDB_MOSAIC_OPEN);
      toolbar->addWidget(load_mosaic_button);

      QToolButton* save_mosaic_button = create_tool_button(L::t(L"Save Mosaic"), IDB_MOSAIC_SAVE);
      toolbar->addWidget(save_mosaic_button);

      QToolButton* export_image_button = create_tool_button(L::t(L"Export Image"), IDB_EXPORT_IMG);
      toolbar->addWidget(export_image_button);

      QToolButton* export_svg_button = create_tool_button(L::t(L"Export SVG"), IDB_EXPORT_SVG);
      toolbar->addWidget(export_svg_button);

      toolbar->addSeparator();

      QToolButton* translate_button = create_tool_button(L::t(L"Translate"), IDB_CANVAS_TRANSLATE);
      translate_button->setCheckable(true);
      toolbar->addWidget(translate_button);

      QToolButton* rotate_button = create_tool_button(L::t(L"Rotate"), IDB_CANVAS_ROTATE);
      rotate_button->setCheckable(true);
      toolbar->addWidget(rotate_button);

      QToolButton* scale_button = create_tool_button(L::t(L"Zoom"), IDB_CANVAS_ZOOM);
      scale_button->setCheckable(true);
      toolbar->addWidget(scale_button);

      QToolButton* redraw_button = create_tool_button(L::t(L"Redraw"), IDB_CANVAS_REDRAW);
      toolbar->addWidget(redraw_button);

      QDockWidget* layers_dock = new QDockWidget(QString::fromWCharArray(L::t(L"Layers")));
      layers_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      QWidget* layers_container = new QWidget();
      QVBoxLayout* layers_layout = new QVBoxLayout(layers_container);

      layers_selector* layer_list = new layers_selector(layers_container, IDB_LAYER_COPY, IDB_LAYER_ADD, IDB_LAYER_DELETE, IDB_LAYER_MOVE_UP, IDB_LAYER_MOVE_DOWN);
      layers_layout->addWidget(layer_list);

      styles_editor* styles_editor = new dak::tiling_ui_qt::styles_editor(layers_container);
      layers_layout->addWidget(styles_editor);

      layers_dock->setWidget(layers_container);

   QDockWidget* figures_dock = new QDockWidget(QString::fromWCharArray(L::t(L"Figures")));
   figures_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
      QWidget* figures_container = new QWidget();
      QVBoxLayout* figures_layout = new QVBoxLayout(figures_container);

      QListWidget* figure_list = new QListWidget(figures_container);
      figure_list->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);
      figure_list->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
      figures_layout->addWidget(figure_list);

      figure_editor* figure_editor = new dak::tiling_ui_qt::figure_editor(figures_container);
      figures_layout->addWidget(figure_editor);

      figures_dock->setWidget(figures_container);

   layered_canvas* canvas = new layered_canvas(nullptr);

   QMainWindow* mainWindow = new QMainWindow;
   mainWindow->setCentralWidget(canvas);
   mainWindow->addToolBar(toolbar);
   mainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, layers_dock);
   mainWindow->addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, figures_dock);
   mainWindow->setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(appInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 256, 256, 0))));

   /////////////////////////////////////////////////////////////////////////
   //
   // The data shown.

   // These will generate tilings and mosaics from files.
   std::vector<std::wstring> errors;
   known_tilings known_tilings(LR"(./tilings)", errors);
   known_mosaics_generator mosaic_gen(LR"(./mosaics)", known_tilings);

   // This will allow to have layers of tilings.
   // Set initial transform to a proper scale.
   dak::ui::layered layered;
   canvas->layered = &layered;
   canvas->transformer.manipulated = &layered;
   layered.set_transform(transform::scale(30));

   /////////////////////////////////////////////////////////////////////////
   //
   // The redraw UI call-backs.

   // This will calculate the region based on the window size and current transform.
   auto window_filling_region = [&canvas]()
   {
      rect region = convert(canvas->geometry());
      region.x = region.y = 0;
      return region.apply(canvas->layered->get_transform().invert());
   };

   auto get_avail_layers = [&]() -> std::vector<std::shared_ptr<layer>>
   {
      return layered.get_layers();
   };

   auto update_layered_transform = [&](const rect& bounds)
   {
      if (bounds.is_invalid())
         return;

      rect region = convert(canvas->geometry());
      double ratio = std::max(region.width / bounds.width, region.height / bounds.height);
      // Make it so we can see 9 instances (3x3) of the tiling or mosaic.
      layered.set_transform(transform::scale(ratio / 3.));
   };

   typedef std::map<std::shared_ptr<mosaic>, map> calculated_mosaics;

   auto find_calculated_mosaic = [&](calculated_mosaics& calc_mos, const std::shared_ptr<mosaic>& mosaic) -> const map&
   {
      for (const auto& calculated : calc_mos)
      {
         if (*(calculated.first) == *mosaic)
         {
            return calculated.second;
         }
      }
      return calc_mos[mosaic] = mosaic->construct(window_filling_region());
   };

   auto update_canvas_layers = [&](const std::vector<std::shared_ptr<layer>>& layers)
   {
      // Optimize updating the layers by only calculating the map of a mosaic once
      // if multiple layers have identical mosaics.
      calculated_mosaics calc_mos;
      for (auto& layer : layers)
      {
         if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
         {
            const auto& calc_map = find_calculated_mosaic(calc_mos, mo_layer->mosaic);
            mo_layer->style->set_map(calc_map);
         }
      }
      canvas->update();
   };

   /////////////////////////////////////////////////////////////////////////
   //
   // The layers UI call-backs.

   auto get_selected_layers = [&]()
   {
      return layer_list->get_selected_layers();
   };

   auto get_selected_styles = [&]() -> std::vector<std::shared_ptr<style>>
   {
      std::vector<std::shared_ptr<style>> selected;
      for (auto layer : layer_list->get_selected_layers())
      {
         if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
         {
            selected.emplace_back(mo_layer->style);
         }
      }
      return selected;
   };

   auto get_selected_mosaics = [&]() -> std::vector<std::shared_ptr<mosaic>>
   {
      std::vector<std::shared_ptr<mosaic>> selected;
      for (auto layer : layer_list->get_selected_layers())
      {
         if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
         {
            selected.emplace_back(mo_layer->mosaic);
         }
      }
      return selected;
   };

   auto find_styles_layers = [&](const std::vector<std::shared_ptr<style>>& styles)
   {
      // Find the layers that corresonpond to the list of styles given.
      std::vector<std::shared_ptr<layer>> layers;
      for (auto& layer : get_avail_layers())
      {
         if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layer))
         {
            if (std::find(styles.begin(), styles.end(), mo_layer->style) != styles.end())
            {
               layers.emplace_back(layer);
            }
         }
      }
      return layers;
   };

   auto update_layer_list = [&]()
   {
      layer_list->update_list_content();
   };

   styles_editor->styles_changed = [&](const styles_editor::styles& styles)
   {
      update_layer_list();
      update_canvas_layers(find_styles_layers(styles));
   };

   /////////////////////////////////////////////////////////////////////////
   //
   // The figures UI call-backs.

   auto get_all_avail_figures = [&]() -> std::vector<std::shared_ptr<figure>>
   {
      std::vector<std::shared_ptr<figure>> avail;
      for (auto mosaic : get_selected_mosaics())
      {
         for (auto& poly_fig : mosaic->tile_figures)
         {
            avail.emplace_back(poly_fig.second);
         }
      }
      return avail;
   };

   auto get_merged_avail_figures = [&]() -> std::vector<std::shared_ptr<figure>>
   {
      std::vector<std::shared_ptr<figure>> avail;
      for (auto figure : get_all_avail_figures())
      {
         bool is_similar = false;
         for (auto existing : avail)
         {
            if (existing->is_similar(*figure))
            {
               is_similar = true;
               break;
            }
         }

         if (is_similar)
            continue;

         avail.emplace_back(figure);
      }
      return avail;
   };

   auto fill_figure_list = [&]()
   {
      const int previousSelection = std::max(0, figure_list->currentRow());
      figure_list->clear();

      for (auto fig : get_merged_avail_figures())
      {
         figure_list->addItem(QString::fromWCharArray(fig->describe().c_str()));
      }

      figure_list->setCurrentRow(std::min(figure_list->count() - 1, previousSelection));
   };

   auto get_selected_figures = [&]() -> std::vector<std::shared_ptr<figure>>
   {
      std::vector<std::shared_ptr<figure>> selected;
      auto avail = get_merged_avail_figures();
      for (int row = 0; row < figure_list->count() && row < avail.size(); ++row)
      {
         const auto item = figure_list->item(row);
         if (item->isSelected())
         {
            selected.emplace_back(avail[row]);
         }
      }
      return selected;
   };

   figure_editor->figure_changed = [&](std::shared_ptr<figure> modified)
   {
      for (auto fig : get_all_avail_figures())
      {
         if (!fig->is_similar(*modified))
            continue;

         fig->make_similar(*modified);
      }

      fill_figure_list();

      update_canvas_layers(get_selected_layers());
   };

   figure_editor->figure_swapped = [&](std::shared_ptr<figure> before, std::shared_ptr<figure> after)
   {
      for (auto mosaic : get_selected_mosaics())
      {
         for (auto& poly_fig : mosaic->tile_figures)
         {
            auto& fig = poly_fig.second;
            if (!fig->is_similar(*before))
               continue;

            fig = after->clone();
         }
      }

      fill_figure_list();

      update_canvas_layers(get_selected_layers());
   };
   

   /////////////////////////////////////////////////////////////////////////
   //
   // The layers and mosaic list contents UI call-backs.

   auto fill_layer_list = [&]()
   {
      layer_list->set_edited(layered.get_layers());
      styles_editor->set_edited(get_selected_styles());
      fill_figure_list();
   };

   auto update_mosaic_map = [&](const std::vector<std::shared_ptr<layer>>& layers, const std::wstring& name)
   {
      layers_dock->setWindowTitle(QString::fromWCharArray(L::t(L"Layers for Mosaic: ")) + QString::fromWCharArray(name.c_str()));

      layered.set_layers(layers);
      if (layers.size() > 0)
         if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic>(layered.get_layers()[0]))
            update_layered_transform(mo_layer->mosaic->tiling.bounds());

      fill_layer_list();

      update_canvas_layers(get_avail_layers());
   };

   /////////////////////////////////////////////////////////////////////////
   //
   // The various data editors UI call-backs.

   layer_list->selection_changed = [&](const layers_selector::layers& layers)
   {
      layered.set_layers(layers);
      styles_editor->set_edited(get_selected_styles());
      fill_figure_list();
      canvas->update();
   };

   layer_list->layers_changed = [&](const layers_selector::layers& layers)
   {
      update_layer_list();
      update_canvas_layers(layers);
   };

   auto add_layer = [&](const std::shared_ptr<mosaic>& new_mosaic)
   {
      auto mo_layer = std::make_shared<styled_mosaic>();
      mo_layer->mosaic = new_mosaic;
      mo_layer->style = std::make_shared<thick>(color(20, 140, 220, 255));
      mo_layer->update_style(window_filling_region());
      auto layers = layered.get_layers();
      const bool was_empty = (layers.size() <= 0);
      layers.emplace_back(mo_layer);
      layered.set_layers(layers);
      fill_layer_list();

      if (was_empty)
         update_layered_transform(mo_layer->mosaic->tiling.bounds());

      canvas->update();
   };

   layer_list->new_layer_requested = [&]()
   {
      auto selector = new tiling_selector(nullptr, add_layer);
      selector->show();
   };

   figure_list->connect(figure_list, &QListWidget::itemSelectionChanged, [&]()
   {
      for (auto figure : get_selected_figures())
      {
         figure_editor->set_edited(figure);
         break;
      }
   });

   /////////////////////////////////////////////////////////////////////////
   //
   // The mosaic tool-bar buttons.

   previous_mosaic_button->connect(previous_mosaic_button, &QPushButton::clicked, [&]()
   {
      mosaic_gen.previous();
      update_mosaic_map(mosaic_gen.generate_current(errors), mosaic_gen.current_name());
   });

   next_mosaic_button->connect(next_mosaic_button, &QPushButton::clicked, [&]()
   {
      mosaic_gen.next();
      update_mosaic_map(mosaic_gen.generate_current(errors), mosaic_gen.current_name());
   });

   load_mosaic_button->connect(load_mosaic_button, &QPushButton::clicked, [&]()
   {
      std::wstring fileName = QFileDialog::getOpenFileName(
         mainWindow, QString::fromWCharArray(L::t(L"Load Mosaic")), QString(),
         QString::fromWCharArray(L::t(L"Mosaic Files (*.tap.txt)"))).toStdWString();
      if (fileName.empty())
         return;
      try
      {
         std::wifstream file(fileName);
         auto layers = read_layered_mosaic(file, mosaic_gen.known_tilings);
         std::experimental::filesystem::path path(fileName);
         update_mosaic_map(layers, path.filename());
      }
      catch (std::exception& ex)
      {
         QErrorMessage error(mainWindow);
         error.showMessage(ex.what());
      }
   });

   save_mosaic_button->connect(save_mosaic_button, &QPushButton::clicked, [&]()
   {
      std::wstring fileName = QFileDialog::getSaveFileName(
         mainWindow, QString::fromWCharArray(L::t(L"Save Mosaic")), QString(),
         QString::fromWCharArray(L::t(L"Mosaic Files (*.tap.txt)"))).toStdWString();
      if (fileName.empty())
         return;
      try
      {
         std::wofstream file(fileName, std::ios::out | std::ios::trunc);
         write_layered_mosaic(file, get_avail_layers());
      }
      catch (std::exception& ex)
      {
         QErrorMessage error(mainWindow);
         error.showMessage(ex.what());
      }
   });

   /////////////////////////////////////////////////////////////////////////
   //
   // The export tool-bar buttons.

   export_image_button->connect(export_image_button, &QPushButton::clicked, [&]()
   {
      auto fileName = QFileDialog::getSaveFileName(
         mainWindow, QString::fromWCharArray(L::t(L"Export Mosaic to an Image")), QString(),
         QString::fromWCharArray(L::t(L"Image Files (*.png *.jpg *.bmp)")));
      if (fileName.isEmpty())
         return;
      canvas->grab().save(fileName);
   });

   export_svg_button->connect(export_svg_button, &QPushButton::clicked, [&]()
   {
      auto fileName = QFileDialog::getSaveFileName(
         mainWindow, QString::fromWCharArray(L::t(L"Export Mosaic to a Scalable Vector Graphics File")), QString(),
         QString::fromWCharArray(L::t(L"SVG Files (*.svg)")));
      if (fileName.isEmpty())
         return;

      QSvgGenerator svg_gen;
      svg_gen.setFileName(fileName);
      svg_gen.setSize(canvas->size());
      svg_gen.setViewBox(QRect(QPoint(0,0), canvas->size()));
      QPainter painter(&svg_gen);
      painter_drawing drw(painter);
      draw_layered(drw, &layered);
   });

   /////////////////////////////////////////////////////////////////////////
   //
   // The canvas manipulation tool-bar buttons.

   auto update_canvas_mode = [&]()
   {
      auto forced_mode = dak::ui::transformer::interaction_mode::normal;
      if (translate_button->isChecked())
         forced_mode = dak::ui::transformer::interaction_mode::moving;
      else if (rotate_button->isChecked())
         forced_mode = dak::ui::transformer::interaction_mode::rotating;
      else if (scale_button->isChecked())
         forced_mode = dak::ui::transformer::interaction_mode::scaling;
      canvas->transformer.forced_interaction_mode = forced_mode;
   };

   translate_button->connect(translate_button, &QPushButton::clicked, [&]()
   {
      if (translate_button->isChecked())
      {
         rotate_button->setChecked(false);
         scale_button->setChecked(false);
      }
      update_canvas_mode();
   });

   rotate_button->connect(rotate_button, &QPushButton::clicked, [&]()
   {
      if (rotate_button->isChecked())
      {
         translate_button->setChecked(false);
         scale_button->setChecked(false);
      }
      update_canvas_mode();
   });

   scale_button->connect(scale_button, &QPushButton::clicked, [&]()
   {
      if (scale_button->isChecked())
      {
         translate_button->setChecked(false);
         rotate_button->setChecked(false);
      }
      update_canvas_mode();
   });

   redraw_button->connect(redraw_button, &QPushButton::clicked, [&]()
   {
      update_canvas_layers(layered.get_layers());
   });

   /////////////////////////////////////////////////////////////////////////
   //
   // Show window and execute app.

   mainWindow->resize(1000, 800);
   mainWindow->show();
   return app->exec();
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
{
   appInstance = hInstance;
   return main(0, 0);
}

