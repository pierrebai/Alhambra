#include <dak/tiling_ui_qt/main_window.h>
#include <dak/tiling_ui_qt/tiling_window.h>
#include <dak/tiling_ui_qt/tiling_selector.h>
#include <dak/tiling_ui_qt/ask.h>

#include <dak/tiling_style/thick.h>
#include <dak/tiling_style/styled_mosaic.h>
#include <dak/tiling_style/style_io.h>

#include <dak/ui/drawing.h>

#include <dak/ui/qt/convert.h>

#include <dak/utility/text.h>

#include <dak/QtAdditions/QtUtilities.h>

#include <QtCore/qstandardpaths.h>
#include <QtCore/qdir.h>
#include <QtGui/qpainter.h>
#include <QtGui/qevent.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qerrormessage.h>
#include <QtWidgets/qtoolbar.h>
#include <QtWinExtras/qwinfunctions.h>
#include <QtSvg/qsvggenerator.h>

namespace dak
{
   namespace tiling_ui_qt
   {
      using namespace dak::geometry;
      using namespace dak::tiling;
      using namespace dak::tiling_style;
      using namespace dak::ui::qt;
      using namespace dak::tiling_ui_qt;
      using namespace dak::QtAdditions;

      using dak::utility::L;

      main_window_t::main_window_t(const main_window_icons_t& icons)
      : known_tilings()
      , mosaic_gen()
      {
         QDir documentFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
         
         add_tilings_from(LR"(./tilings)");
         add_tilings_from(documentFolder.absoluteFilePath("tilings").toStdWString());

         mosaic_gen.add_folder(LR"(./mosaics)");
         mosaic_gen.add_folder(documentFolder.absoluteFilePath("mosaics").toStdWString());

         build_ui(icons);
         fill_ui();
         connect_ui(icons);
      }

      void main_window_t::add_tilings_from(const std::wstring& folder)
      {
         const auto new_tilings = read_tilings(folder, errors);
         known_tilings.insert(known_tilings.end(), new_tilings.begin(), new_tilings.end());
      }

      // Create the UI elements.
      void main_window_t::build_ui(const main_window_icons_t& icons)
      {
         QToolBar* toolbar = new QToolBar();
            toolbar->setIconSize(QSize(32, 32));

            previous_mosaic_action = CreateAction(L::t(L"Previous Mosaic"), icons.mosaic_previous, QKeySequence(QKeySequence::StandardKey::Back));
            previous_mosaic_button = CreateToolButton(previous_mosaic_action);
            toolbar->addWidget(previous_mosaic_button);

            next_mosaic_action = CreateAction(L::t(L"Next Mosaic"), icons.mosaic_next, QKeySequence(QKeySequence::StandardKey::Forward));
            next_mosaic_button = CreateToolButton(next_mosaic_action);
            toolbar->addWidget(next_mosaic_button);

            toolbar->addSeparator();

            undo_action = CreateAction(L::t(L"Undo"), icons.undo, QKeySequence(QKeySequence::StandardKey::Undo));
            undo_button = CreateToolButton(undo_action);
            undo_action->setEnabled(false);
            toolbar->addWidget(undo_button);

            redo_action = CreateAction(L::t(L"Redo"), icons.redo, QKeySequence(QKeySequence::StandardKey::Redo));
            redo_button = CreateToolButton(redo_action);
            redo_action->setEnabled(false);
            toolbar->addWidget(redo_button);

            toolbar->addSeparator();

            load_mosaic_action = CreateAction(L::t(L"Load Mosaic"), icons.mosaic_open, QKeySequence(QKeySequence::StandardKey::Open));
            load_mosaic_button = CreateToolButton(load_mosaic_action);
            toolbar->addWidget(load_mosaic_button);

            save_mosaic_action = CreateAction(L::t(L"Save Mosaic"), icons.mosaic_save, QKeySequence(QKeySequence::StandardKey::Save));
            save_mosaic_button = CreateToolButton(save_mosaic_action);
            toolbar->addWidget(save_mosaic_button);

            tiling_editor_action = CreateAction(L::t(L"Tiling Editor"), icons.tiling_editor);
            tiling_editor_button = CreateToolButton(tiling_editor_action);
            toolbar->addWidget(tiling_editor_button);

            export_image_action = CreateAction(L::t(L"Export Image"), icons.export_img);
            export_image_button = CreateToolButton(export_image_action);
            toolbar->addWidget(export_image_button);

            export_svg_action = CreateAction(L::t(L"Export SVG"), icons.export_svg);
            export_svg_button = CreateToolButton(export_svg_action);
            toolbar->addWidget(export_svg_button);

            toolbar->addSeparator();

            translate_action = CreateAction(L::t(L"Pan"), icons.canvas_translate);
            translate_button = CreateToolButton(translate_action);
            translate_action->setCheckable(true);
            toolbar->addWidget(translate_button);

            rotate_action = CreateAction(L::t(L"Rotate"), icons.canvas_rotate);
            rotate_button = CreateToolButton(rotate_action);
            rotate_action->setCheckable(true);
            toolbar->addWidget(rotate_button);

            scale_action = CreateAction(L::t(L"Zoom"), icons.canvas_zoom);
            scale_button = CreateToolButton(scale_action);
            scale_action->setCheckable(true);
            toolbar->addWidget(scale_button);

            redraw_action = CreateAction(L::t(L"Redraw"), icons.canvas_redraw, QKeySequence(QKeySequence::StandardKey::Refresh));
            redraw_button = CreateToolButton(redraw_action);
            toolbar->addWidget(redraw_button);

            layers_dock = new QDockWidget(QString::fromWCharArray(L::t(L"Layers")));
            layers_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
            QWidget* layers_container = new QWidget();
            QVBoxLayout* layers_layout = new QVBoxLayout(layers_container);

            layer_list = new layers_selector_t(layers_container, icons.layer_copy, icons.layer_add, icons.layer_delete, icons.layer_move_up, icons.layer_move_down);
            layers_layout->addWidget(layer_list);

            styles_editor = new dak::tiling_ui_qt::styles_editor_t(layers_container);
            layers_layout->addWidget(styles_editor);

            layers_dock->setWidget(layers_container);

         QDockWidget* figures_dock = new QDockWidget(QString::fromWCharArray(L::t(L"Figures")));
         figures_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
            QWidget* figures_container = new QWidget();
            QVBoxLayout* figures_layout = new QVBoxLayout(figures_container);

            figure_list = new dak::tiling_ui_qt::figure_selector_t(figures_container);
            figures_layout->addWidget(figure_list);

            figure_editor = new dak::tiling_ui_qt::figure_editor_t(figures_container);
            figures_layout->addWidget(figure_editor);

            figures_dock->setWidget(figures_container);

         canvas = new layered_canvas_t(nullptr);
         canvas->transformer.mouse_interaction_modifier = ui::modifiers_t::none;

         setCentralWidget(canvas);
         addToolBar(toolbar);
         addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, layers_dock);
         addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, figures_dock);
         setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(icons.app_icon), IMAGE_ICON, 256, 256, 0))));
      }

      // Connect the signals of the UI elements.
      void main_window_t::connect_ui(const tiling_editor_icons_t& icons)
      {
         /////////////////////////////////////////////////////////////////////////
         //
         // Undo / redo actions.

         undo_action->connect(undo_action, &QAction::triggered, [&]()
         {
            undo_stack.undo();
            update_undo_redo_actions();
         });

         redo_action->connect(redo_action, &QAction::triggered, [&]()
         {
            undo_stack.redo();
            update_undo_redo_actions();
         });

         /////////////////////////////////////////////////////////////////////////
         //
         // Load / save actions.

         previous_mosaic_action->connect(previous_mosaic_action, &QAction::triggered, [self=this]()
         {
            if (!self->save_if_required(L::t(L"load a mosaic"), L::t(L"loading a mosaic")))
               return;

            self->mosaic_gen.previous();
            self->clear_undo_stack();
            self->update_mosaic_map(self->mosaic_gen.generate_current(self->known_tilings, self->errors), self->mosaic_gen.current_name());
         });

         next_mosaic_action->connect(next_mosaic_action, &QAction::triggered, [self=this]()
         {
            if (!self->save_if_required(L::t(L"load a mosaic"), L::t(L"loading a mosaic")))
               return;

            self->mosaic_gen.next();
            self->clear_undo_stack();
            self->update_mosaic_map(self->mosaic_gen.generate_current(self->known_tilings, self->errors), self->mosaic_gen.current_name());
         });

         load_mosaic_action->connect(load_mosaic_action, &QAction::triggered, [self=this]()
         {
            if (!self->save_if_required(L::t(L"load a mosaic"), L::t(L"loading a mosaic")))
               return;

            std::filesystem::path path;
            auto layers = ask_open_layered_mosaic(self->known_tilings, path, self);
            if (layers.size() == 0)
               return;
            self->clear_undo_stack();
            self->update_mosaic_map(layers, path.filename());
         });

         save_mosaic_action->connect(save_mosaic_action, &QAction::triggered, [self=this]()
         {
            self->save_mosaic();
         });

         tiling_editor_action->connect(tiling_editor_action, &QAction::triggered, [self=this,&icons=icons, &known_tilings=known_tilings]()
         {
            auto window = new tiling_window_t(known_tilings, icons, self);
            window->resize(1200, 900);
            window->show();
         });

         /////////////////////////////////////////////////////////////////////////
         //
         // The export tool-bar actions.

         export_image_action->connect(export_image_action, &QAction::triggered, [self=this]()
         {
            auto fileName = ask_save(L::t(L"Export Mosaic to an Image"), L::t(L"Image Files (*.png *.jpg *.bmp)"), self);
            if (fileName.empty())
               return;

            self->canvas->grab().save(QString::fromWCharArray(fileName.c_str()));
         });

         export_svg_action->connect(export_svg_action, &QAction::triggered, [self=this]()
         {
            auto fileName = ask_save(L::t(L"Export Mosaic to a Scalable Vector Graphics File"), L::t(L"SVG Files (*.svg)"), self);
            if (fileName.empty())
               return;

            QSvgGenerator svg_gen;
            svg_gen.setFileName(QString::fromWCharArray(fileName.c_str()));
            svg_gen.setSize(self->canvas->size());
            svg_gen.setViewBox(QRect(QPoint(0,0), self->canvas->size()));
            QPainter painter(&svg_gen);
            painter_drawing_t drw(painter);
            draw_layered(drw, &self->layered);
         });

         /////////////////////////////////////////////////////////////////////////
         //
         // The style editor UI call-backs.

         styles_editor->styles_changed = [self=this](const styles_editor_t::styles_t& styles, bool interacting)
         {
            self->update_layer_list();

            if (!interacting)
               self->commit_to_undo();

            self->update_canvas_layers(self->find_styles_layers(styles));
         };

         /////////////////////////////////////////////////////////////////////////
         //
         // The layer list UI call-backs.

         layer_list->layers_changed = [self=this](const layers_selector_t::layers& layers)
         {
            self->update_layer_list();
            self->styles_editor->set_edited(self->get_selected_styles());
            self->commit_to_undo();
            self->layered.set_layers(layers);
            self->update_canvas_layers(layers);
         };

         layer_list->selection_changed = [self=this](const layers_selector_t::layers& layers)
         {
            self->layered.set_layers(layers);
            self->styles_editor->set_edited(self->get_selected_styles());
            self->fill_figure_list();
            self->canvas->update();
         };

         layer_list->new_layer_requested = [&known_tilings=this->known_tilings, &icons, self=this]()
         {
            auto selector = new tiling_selector_t(known_tilings, icons, nullptr, [self=self](const std::shared_ptr<mosaic_t>& new_mosaic) { self->add_layer(new_mosaic); });
            selector->show();
         };

         /////////////////////////////////////////////////////////////////////////
         //
         // The figures list UI call-backs.

         figure_list->figure_changed = [self=this](std::shared_ptr<figure_t> modified)
         {
            for (auto fig : self->get_all_avail_figures())
            {
               if (!fig->is_similar(*modified))
                  continue;

               fig->make_similar(*modified);
            }

            const bool force_update = true;
            self->fill_figure_editor(force_update);
            self->fill_figure_list();

            self->commit_to_undo();

            self->update_canvas_layers(self->get_selected_layers());
         };

         figure_list->figure_swapped = [self=this](std::shared_ptr<figure_t> before, std::shared_ptr<figure_t> after)
         {
            for (auto mosaic : self->get_selected_mosaics())
            {
               for (auto& poly_fig : mosaic->tile_figures)
               {
                  auto& fig = poly_fig.second;
                  if (!fig->is_similar(*before))
                     continue;

                  fig = after->clone();
               }
            }

            self->figure_editor->set_edited(after);
            self->fill_figure_list();
            self->commit_to_undo();

            self->update_canvas_layers(self->get_selected_layers());
         };

         figure_list->selection_changed = [self=this](const std::shared_ptr<figure_t>& figure)
         {
            self->fill_figure_editor();
         };

         /////////////////////////////////////////////////////////////////////////
         //
         // The figure editor UI call-backs.

         figure_editor->figure_changed = [self=this](std::shared_ptr<figure_t> modified, bool interacting)
         {
            for (auto fig : self->get_all_avail_figures())
            {
               if (!fig->is_similar(*modified))
                  continue;

               fig->make_similar(*modified);
            }

            self->fill_figure_list();

            if (!interacting)
               self->commit_to_undo();

            self->update_canvas_layers(self->get_selected_layers());
         };

         /////////////////////////////////////////////////////////////////////////
         //
         // Canvas mode actions.

         translate_action->connect(translate_action, &QAction::triggered, [&]()
         {
            if (translate_action->isChecked())
            {
               rotate_action->setChecked(false);
               scale_action->setChecked(false);
            }
            update_canvas_mode();
         });

         rotate_action->connect(rotate_action, &QAction::triggered, [&]()
         {
            if (rotate_action->isChecked())
            {
               translate_action->setChecked(false);
               scale_action->setChecked(false);
            }
            update_canvas_mode();
         });

         scale_action->connect(scale_action, &QAction::triggered, [&]()
         {
            if (scale_action->isChecked())
            {
               translate_action->setChecked(false);
               rotate_action->setChecked(false);
            }
            update_canvas_mode();
         });

         redraw_action->connect(redraw_action, &QAction::triggered, [&]()
         {
            update_canvas_layers(layered.get_layers());
         });
      }

      // Fill the UI with the intial data.
      void main_window_t::fill_ui()
      {
         canvas->layered = &layered;
         canvas->transformer.manipulated = &layered;
         layered.set_transform(transform_t::scale(30));
      }

      void main_window_t::closeEvent(QCloseEvent* ev)
      {
         if (save_if_required(L::t(L"close the window"), L::t(L"closing the window")))
            QWidget::closeEvent(ev);
         else
            ev->ignore();
      }

      bool main_window_t::save_if_required(const std::wstring& action, const std::wstring& actioning)
      {
         if (undo_stack.has_undo())
         {
            yes_no_cancel_t answer = ask_yes_no_cancel(
               L::t(L"Unsaved Mosaic Warning"),
               std::wstring(L::t(L"The current mosaic has not been saved.\nDo you want to save it before ")) + actioning + L::t(L"?"),
               this);
            if (answer == yes_no_cancel_t::cancel)
               return false;
            else if (answer == yes_no_cancel_t::yes)
               if (!save_mosaic())
                  return false;
         }

         return true;
      }

      bool main_window_t::save_mosaic()
      {
         std::filesystem::path path;
         return ask_save_layered_mosaic(get_avail_layers(), path, this);
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The redraw UI call-backs.

      // This will calculate the region based on the window size and current transform.
      geometry::rectangle_t main_window_t::window_filling_region()
      {
         geometry::rectangle_t region = convert(canvas->geometry());
         region.x = region.y = 0;
         return region.apply(canvas->layered->get_transform().invert());
      }

      std::vector<std::shared_ptr<layer_t>> main_window_t::get_avail_layers()
      {
         return layered.get_layers();
      }

      void main_window_t::update_layered_transform(const geometry::rectangle_t& bounds)
      {
         if (bounds.is_invalid())
            return;

         geometry::rectangle_t region = convert(canvas->geometry());
         double ratio = std::max(region.width / bounds.width, region.height / bounds.height);
         // Make it so we can see 9 instances (3x3) of the tiling or mosaic.
         layered.set_transform(transform_t::scale(ratio / 3.));
      }

      const geometry::edges_map_t& main_window_t::find_calculated_mosaic(calculated_mosaics& calc_mos, const std::shared_ptr<mosaic_t>& mosaic)
      {
         for (const auto& calculated : calc_mos)
         {
            if (*(calculated.first) == *mosaic)
            {
               return calculated.second;
            }
         }
         return calc_mos[mosaic] = mosaic->construct(window_filling_region());
      }

      void main_window_t::update_canvas_layers(const std::vector<std::shared_ptr<layer_t>>& layers)
      {
         // Optimize updating the layers by only calculating the map of a mosaic once
         // if multiple layers have identical mosaics.
         calculated_mosaics calc_mos;
         for (auto& layer : layers)
         {
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               const auto& calc_map = find_calculated_mosaic(calc_mos, mo_layer->mosaic);
               mo_layer->style->set_map(calc_map);
            }
         }
         canvas->update();
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The layers UI call-backs.

      std::vector<std::shared_ptr<layer_t>> main_window_t::get_selected_layers()
      {
         return layer_list->get_selected_layers();
      }

      std::vector<std::shared_ptr<style_t>> main_window_t::get_selected_styles()
      {
         std::vector<std::shared_ptr<tiling_style::style_t>> selected;
         for (auto layer : layer_list->get_selected_layers())
         {
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               selected.emplace_back(mo_layer->style);
            }
         }
         return selected;
      }

      std::vector<std::shared_ptr<mosaic_t>> main_window_t::get_selected_mosaics()
      {
         std::vector<std::shared_ptr<mosaic_t>> selected;
         for (auto layer : layer_list->get_selected_layers())
         {
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               selected.emplace_back(mo_layer->mosaic);
            }
         }
         return selected;
      }

      std::vector<std::shared_ptr<layer_t>> main_window_t::find_styles_layers(const std::vector<std::shared_ptr<tiling_style::style_t>>& styles)
      {
         // Find the layers that corresonpond to the list of styles given.
         std::vector<std::shared_ptr<layer_t>> layers;
         for (auto& layer : get_avail_layers())
         {
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               if (std::find(styles.begin(), styles.end(), mo_layer->style) != styles.end())
               {
                  layers.emplace_back(layer);
               }
            }
         }
         return layers;
      }

      void main_window_t::update_layer_list()
      {
         layer_list->update_list_content();
      }

      void main_window_t::fill_layer_list()
      {
         layer_list->set_edited(layered.get_layers());
         styles_editor->set_edited(get_selected_styles());
         fill_figure_list();
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The figures list filling.

      std::vector<std::shared_ptr<figure_t>> main_window_t::get_all_avail_figures()
      {
         std::vector<std::shared_ptr<figure_t>> avail;
         for (auto mosaic : get_selected_mosaics())
         {
            for (auto& poly_fig : mosaic->tile_figures)
            {
               avail.emplace_back(poly_fig.second);
            }
         }
         return avail;
      }

      std::vector<std::shared_ptr<figure_t>> main_window_t::get_merged_avail_figures()
      {
         std::vector<std::shared_ptr<figure_t>> avail;
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
      }

      void main_window_t::fill_figure_list()
      {
         figure_list->set_edited(get_merged_avail_figures());
      }

      std::shared_ptr<figure_t> main_window_t::get_selected_figure()
      {
         return figure_list->get_selected_figure();
      }

      void main_window_t::fill_figure_editor(bool force_update)
      {
         if (const auto& figure = get_selected_figure())
         {
            figure_editor->set_edited(figure, force_update);
         }
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // Undo / redo tool-bar buttons.

      void main_window_t::deaden_styled_mosaic(std::any& data)
      {
         auto& layers = std::any_cast<dak::ui::layered_t::layers_t&>(data);
         for (auto& layer : layers)
         {
            if (auto style = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               style->style->set_map(edges_map_t());
            }
         }
      }

      void main_window_t::update_undo_redo_actions()
      {
         undo_action->setEnabled(undo_stack.has_undo());
         redo_action->setEnabled(undo_stack.has_redo());
      }

      void main_window_t::awaken_styled_mosaic(const std::any& data)
      {
         dak::ui::layered_t::layers_t layers = clone_layers(std::any_cast<const dak::ui::layered_t::layers_t&>(data));
         for (auto& layer : layers)
         {
            if (auto style = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               style->style->set_map(style->mosaic->construct(window_filling_region()));
            }
         }

         layered.set_layers(layers);

         fill_layer_list();

         const bool force_update = true;
         fill_figure_editor(force_update);

         update_canvas_layers(layered.get_layers());
      }

      void main_window_t::awaken_to_empty_canvas()
      {
         layered.set_layers({});

         fill_layer_list();

         const bool force_update = true;
         fill_figure_editor(force_update);

         update_canvas_layers(layered.get_layers());
      }

      void main_window_t::clear_undo_stack()
      {
         undo_stack.clear();
      }

      void main_window_t::commit_to_undo()
      {
         const dak::ui::layered_t::layers_t& layers = layered.get_layers();
         undo_stack.simple_commit(
         {
            clone_layers(layers),
            [self=this](std::any& data) { self->deaden_styled_mosaic(data); },
            [self=this](const std::any& data) { self->awaken_styled_mosaic(data); }
         });
         update_undo_redo_actions();
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // Layer manipulations.

      dak::ui::layered_t::layers_t main_window_t::clone_layers(const dak::ui::layered_t::layers_t& layers)
      {
         dak::ui::layered_t::layers_t cloned_layers;
         for (const auto& layer : layers)
            cloned_layers.emplace_back(layer->clone());
         return cloned_layers;
      }

      void main_window_t::add_layer(const std::shared_ptr<mosaic_t>& new_mosaic)
      {
         auto mo_layer = std::make_shared<styled_mosaic_t>();
         mo_layer->mosaic = new_mosaic;
         mo_layer->style = std::make_shared<thick_t>(ui::color_t(20, 140, 220, 255));
         mo_layer->update_style(window_filling_region());
         auto layers = layered.get_layers();
         const bool was_empty = (layers.size() <= 0);
         layers.emplace_back(mo_layer);
         layered.set_layers(layers);
         fill_layer_list();

         if (was_empty)
         {
            update_layered_transform(mo_layer->mosaic->tiling.bounds());
            clear_undo_stack();
            // Note: when adding layers, allow undoing back to an empty canvas.
            undo_stack.simple_commit({ 0, nullptr, [self=this](const std::any&) { self->awaken_to_empty_canvas(); } });
         }

         commit_to_undo();

         canvas->update();
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The mosaic tool-bar buttons.

      void main_window_t::update_mosaic_map(const std::vector<std::shared_ptr<layer_t>>& layers, const std::wstring& name)
      {
         layers_dock->setWindowTitle(QString::fromWCharArray(L::t(L"Layers for Mosaic: ")) + QString::fromWCharArray(name.c_str()));

         layered.set_layers(layers);
         if (layers.size() > 0)
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(layered.get_layers()[0]))
               update_layered_transform(mo_layer->mosaic->tiling.bounds());

         fill_layer_list();
         commit_to_undo();

         update_canvas_layers(get_avail_layers());
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The canvas manipulation tool-bar buttons.

      void main_window_t::update_canvas_mode()
      {
         auto forced_mode = dak::ui::transformer_t::interaction_mode_t::normal;
         if (translate_button->isChecked())
            forced_mode = dak::ui::transformer_t::interaction_mode_t::moving;
         else if (rotate_button->isChecked())
            forced_mode = dak::ui::transformer_t::interaction_mode_t::rotating;
         else if (scale_button->isChecked())
            forced_mode = dak::ui::transformer_t::interaction_mode_t::scaling;
         canvas->transformer.forced_interaction_mode = forced_mode;
      }

   }
}
