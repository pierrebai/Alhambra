#include <dak/tiling_ui_qt/main_window.h>
#include <dak/tiling_ui_qt/tiling_window.h>
#include <dak/tiling_ui_qt/tiling_selector.h>
#include <dak/tiling_ui_qt/ask.h>

#include <dak/tiling_style/thick.h>
#include <dak/tiling_style/styled_mosaic.h>
#include <dak/tiling_style/mosaic_io.h>

#include <dak/ui/drawing.h>

#include <dak/ui/qt/convert.h>

#include <dak/utility/text.h>

#include <dak/QtAdditions/QtUtilities.h>

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
      : my_known_tilings()
      , my_mosaic_gen()
      , my_layered(new ui::layered_t)
      , my_original_mosaic(new ui::layered_t)
      {
         add_tilings_from(LR"(./tilings)");
         add_tilings_from(get_user_tilings_old_folder());
         add_tilings_from(get_user_tilings_folder());

         my_mosaic_gen.add_folder(LR"(./mosaics)");
         my_mosaic_gen.add_folder(get_user_mosaics_old_folder());
         my_mosaic_gen.add_folder(get_user_mosaics_folder());

         build_ui(icons);
         fill_ui();
         connect_ui(icons);
      }

      void main_window_t::add_tilings_from(const std::wstring& folder)
      {
         const auto new_tilings = read_tilings(folder, my_errors);
         my_known_tilings.insert(new_tilings.begin(), new_tilings.end());
      }

      // Create the UI elements.
      void main_window_t::build_ui(const main_window_icons_t& icons)
      {
         QToolBar* toolbar = new QToolBar();
            toolbar->setIconSize(QSize(32, 32));

            my_previous_mosaic_action = CreateAction(L::t(L"Previous Mosaic"), icons.mosaic_previous, QKeySequence(QKeySequence::StandardKey::Back));
            my_previous_mosaic_button = CreateToolButton(my_previous_mosaic_action);
            toolbar->addWidget(my_previous_mosaic_button);

            my_next_mosaic_action = CreateAction(L::t(L"Next Mosaic"), icons.mosaic_next, QKeySequence(QKeySequence::StandardKey::Forward));
            my_next_mosaic_button = CreateToolButton(my_next_mosaic_action);
            toolbar->addWidget(my_next_mosaic_button);

            toolbar->addSeparator();

            my_undo_action = CreateAction(L::t(L"Undo"), icons.undo, QKeySequence(QKeySequence::StandardKey::Undo));
            my_undo_button = CreateToolButton(my_undo_action);
            my_undo_action->setEnabled(false);
            toolbar->addWidget(my_undo_button);

            my_redo_action = CreateAction(L::t(L"Redo"), icons.redo, QKeySequence(QKeySequence::StandardKey::Redo));
            my_redo_button = CreateToolButton(my_redo_action);
            my_redo_action->setEnabled(false);
            toolbar->addWidget(my_redo_button);

            toolbar->addSeparator();

            my_load_mosaic_action = CreateAction(L::t(L"Load Mosaic"), icons.mosaic_open, QKeySequence(QKeySequence::StandardKey::Open));
            my_load_mosaic_button = CreateToolButton(my_load_mosaic_action);
            toolbar->addWidget(my_load_mosaic_button);

            my_save_mosaic_action = CreateAction(L::t(L"Save Mosaic"), icons.mosaic_save, QKeySequence(QKeySequence::StandardKey::Save));
            my_save_mosaic_button = CreateToolButton(my_save_mosaic_action);
            toolbar->addWidget(my_save_mosaic_button);

            my_tiling_editor_action = CreateAction(L::t(L"Tiling Editor"), icons.tiling_editor);
            my_tiling_editor_button = CreateToolButton(my_tiling_editor_action);
            toolbar->addWidget(my_tiling_editor_button);

            my_export_image_action = CreateAction(L::t(L"Export Image"), icons.export_img);
            my_export_image_button = CreateToolButton(my_export_image_action);
            toolbar->addWidget(my_export_image_button);

            my_export_svg_action = CreateAction(L::t(L"Export SVG"), icons.export_svg);
            my_export_svg_button = CreateToolButton(my_export_svg_action);
            toolbar->addWidget(my_export_svg_button);

            toolbar->addSeparator();

            my_translate_action = CreateAction(L::t(L"Pan"), icons.canvas_translate);
            my_translate_button = CreateToolButton(my_translate_action);
            my_translate_action->setCheckable(true);
            toolbar->addWidget(my_translate_button);

            my_rotate_action = CreateAction(L::t(L"Rotate"), icons.canvas_rotate);
            my_rotate_button = CreateToolButton(my_rotate_action);
            my_rotate_action->setCheckable(true);
            toolbar->addWidget(my_rotate_button);

            my_scale_action = CreateAction(L::t(L"Zoom"), icons.canvas_zoom);
            my_scale_button = CreateToolButton(my_scale_action);
            my_scale_action->setCheckable(true);
            toolbar->addWidget(my_scale_button);

            my_redraw_action = CreateAction(L::t(L"Redraw"), icons.canvas_redraw, QKeySequence(QKeySequence::StandardKey::Refresh));
            my_redraw_button = CreateToolButton(my_redraw_action);
            toolbar->addWidget(my_redraw_button);

         my_layers_dock = new QDockWidget(QString::fromWCharArray(L::t(L"Layers")));
         my_layers_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
            QWidget* layers_container = new QWidget();
            QVBoxLayout* layers_layout = new QVBoxLayout(layers_container);

            my_layer_list = new layers_selector_t(layers_container, icons);
            layers_layout->addWidget(my_layer_list);

            my_styles_editor = new dak::tiling_ui_qt::styles_editor_t(layers_container);
            layers_layout->addWidget(my_styles_editor);

            my_layers_dock->setWidget(layers_container);

         QDockWidget* figures_dock = new QDockWidget(QString::fromWCharArray(L::t(L"Figures")));
         figures_dock->setFeatures(QDockWidget::DockWidgetFeature::DockWidgetFloatable | QDockWidget::DockWidgetFeature::DockWidgetMovable);
            QWidget* figures_container = new QWidget();
            QVBoxLayout* figures_layout = new QVBoxLayout(figures_container);

            my_figure_list = new dak::tiling_ui_qt::figure_selector_t(figures_container);
            figures_layout->addWidget(my_figure_list);

            my_figure_editor = new dak::tiling_ui_qt::figure_editor_t(figures_container);
            figures_layout->addWidget(my_figure_editor);

            figures_dock->setWidget(figures_container);

         my_layered_canvas = new layered_canvas_t(nullptr);
         my_layered_canvas->transformer.mouse_interaction_modifier = ui::modifiers_t::none;

         setCentralWidget(my_layered_canvas);
         addToolBar(toolbar);
         addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, my_layers_dock);
         addDockWidget(Qt::DockWidgetArea::LeftDockWidgetArea, figures_dock);
         setWindowIcon(QIcon(QtWin::fromHICON((HICON)::LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(icons.app_icon), IMAGE_ICON, 256, 256, 0))));
         resizeDocks({ my_layers_dock , figures_dock }, { 400, 400 }, Qt::Horizontal);
      }

      // Connect the signals of the UI elements.
      void main_window_t::connect_ui(const tiling_editor_icons_t& icons)
      {
         /////////////////////////////////////////////////////////////////////////
         //
         // Undo / redo actions.

         my_undo_action->connect(my_undo_action, &QAction::triggered, [&]()
         {
            my_undo_stack.undo();
            update_undo_redo_actions();
         });

         my_redo_action->connect(my_redo_action, &QAction::triggered, [&]()
         {
            my_undo_stack.redo();
            update_undo_redo_actions();
         });

         /////////////////////////////////////////////////////////////////////////
         //
         // Load / save actions.

         my_previous_mosaic_action->connect(my_previous_mosaic_action, &QAction::triggered, [self=this]()
         {
            if (!self->save_if_required(L::t(L"load a mosaic"), L::t(L"loading a mosaic")))
               return;

            self->my_mosaic_gen.previous();
            self->clear_undo_stack();
            self->update_mosaic_map(self->my_mosaic_gen.generate_current(self->my_known_tilings, self->my_errors), self->my_mosaic_gen.current_name());
         });

         my_next_mosaic_action->connect(my_next_mosaic_action, &QAction::triggered, [self=this]()
         {
            if (!self->save_if_required(L::t(L"load a mosaic"), L::t(L"loading a mosaic")))
               return;

            self->my_mosaic_gen.next();
            self->clear_undo_stack();
            self->update_mosaic_map(self->my_mosaic_gen.generate_current(self->my_known_tilings, self->my_errors), self->my_mosaic_gen.current_name());
         });

         my_load_mosaic_action->connect(my_load_mosaic_action, &QAction::triggered, [self=this]()
         {
            if (!self->save_if_required(L::t(L"load a mosaic"), L::t(L"loading a mosaic")))
               return;

            std::filesystem::path path;
            auto layers = ask_open_layered_mosaic(self->my_known_tilings, path, self);
            if (layers.size() == 0)
               return;
            self->clear_undo_stack();
            self->update_mosaic_map(layers, path.filename());
         });

         my_save_mosaic_action->connect(my_save_mosaic_action, &QAction::triggered, [self=this]()
         {
            self->save_mosaic();
         });

         my_tiling_editor_action->connect(my_tiling_editor_action, &QAction::triggered, [self=this,&icons=icons, &my_known_tilings=my_known_tilings]()
         {
            auto window = new tiling_window_t(my_known_tilings, icons, self);
            window->resize(1200, 900);
            window->show();
         });

         /////////////////////////////////////////////////////////////////////////
         //
         // The export tool-bar actions.

         my_export_image_action->connect(my_export_image_action, &QAction::triggered, [self=this]()
         {
            auto fileName = ask_save(L::t(L"Export Mosaic to an Image"), L::t(L"Image Files (*.png *.jpg *.bmp)"), self);
            if (fileName.empty())
               return;

            self->my_layered_canvas->grab().save(QString::fromWCharArray(fileName.c_str()));
         });

         my_export_svg_action->connect(my_export_svg_action, &QAction::triggered, [self=this]()
         {
            auto fileName = ask_save(L::t(L"Export Mosaic to a Scalable Vector Graphics File"), L::t(L"SVG Files (*.svg)"), self);
            if (fileName.empty())
               return;

            QSvgGenerator svg_gen;
            svg_gen.setFileName(QString::fromWCharArray(fileName.c_str()));
            svg_gen.setSize(self->my_layered_canvas->size());
            svg_gen.setViewBox(QRect(QPoint(0,0), self->my_layered_canvas->size()));
            QPainter painter(&svg_gen);
            painter_drawing_t drw(painter);
            draw_layered(drw, self->my_layered);
         });

         /////////////////////////////////////////////////////////////////////////
         //
         // The style editor UI call-backs.

         my_styles_editor->styles_changed = [self=this](const styles_editor_t::styles_t& styles, bool interacting)
         {
            self->update_layer_list();

            if (!interacting)
               self->commit_to_undo();

            self->update_canvas_layers(self->find_styles_layers(styles));
         };

         /////////////////////////////////////////////////////////////////////////
         //
         // The layer list UI call-backs.

         my_layer_list->layers_changed = [self=this](const layers_selector_t::layers& layers)
         {
            self->update_layer_list();
            self->my_styles_editor->set_edited(self->get_selected_styles());
            self->commit_to_undo();
            self->my_layered->set_layers(layers);
            self->update_canvas_layers(layers);
         };

         my_layer_list->selection_changed = [self=this](const layers_selector_t::layers& layers)
         {
            self->my_layered->set_layers(layers);
            self->my_styles_editor->set_edited(self->get_selected_styles());
            self->fill_figure_list();
            self->my_layered_canvas->update();
         };

         my_layer_list->new_layer_requested = [&my_known_tilings=this->my_known_tilings, &icons, self=this]()
         {
            auto selector = new tiling_selector_t(my_known_tilings, icons, nullptr, [self=self](const std::shared_ptr<tiling_t>& tiling)
            {
               auto mosaic = generate_mosaic(tiling);
               if (!mosaic)
                  return;
               self->add_layer(mosaic);
            });
            selector->show();
         };

         /////////////////////////////////////////////////////////////////////////
         //
         // The figures list UI call-backs.

         my_figure_list->figure_changed = [self=this](std::shared_ptr<figure_t> modified)
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

         my_figure_list->figure_swapped = [self=this](std::shared_ptr<figure_t> before, std::shared_ptr<figure_t> after)
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

            self->my_figure_editor->set_edited(after);
            self->fill_figure_list();
            self->commit_to_undo();

            self->update_canvas_layers(self->get_selected_layers());
         };

         my_figure_list->selection_changed = [self=this](const std::shared_ptr<figure_t>& figure)
         {
            self->fill_figure_editor();
         };

         /////////////////////////////////////////////////////////////////////////
         //
         // The figure editor UI call-backs.

         my_figure_editor->figure_changed = [self=this](std::shared_ptr<figure_t> modified, bool interacting)
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

         my_translate_action->connect(my_translate_action, &QAction::triggered, [&]()
         {
            if (my_translate_action->isChecked())
            {
               my_rotate_action->setChecked(false);
               my_scale_action->setChecked(false);
            }
            update_canvas_mode();
         });

         my_rotate_action->connect(my_rotate_action, &QAction::triggered, [&]()
         {
            if (my_rotate_action->isChecked())
            {
               my_translate_action->setChecked(false);
               my_scale_action->setChecked(false);
            }
            update_canvas_mode();
         });

         my_scale_action->connect(my_scale_action, &QAction::triggered, [&]()
         {
            if (my_scale_action->isChecked())
            {
               my_translate_action->setChecked(false);
               my_rotate_action->setChecked(false);
            }
            update_canvas_mode();
         });

         my_redraw_action->connect(my_redraw_action, &QAction::triggered, [&]()
         {
            update_canvas_layers(my_layered->get_layers());
         });
      }

      // Fill the UI with the intial data.
      void main_window_t::fill_ui()
      {
         my_layered_canvas->layered = my_layered;
         my_layered_canvas->transformer.manipulated = my_layered_canvas;
         my_layered->set_transform(transform_t::identity());
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
         if (my_layered->get_layers().size() > 0 && *my_layered != *my_original_mosaic)
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
         if (!ask_save_layered_mosaic(get_avail_mosaics(), path, this))
            return false;

         my_original_mosaic->make_similar(*my_layered);
         return true;
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The redraw UI call-backs.

      // This will calculate the region needed to fill the window for a given
      // mosaic based on the window size and current transform.
      geometry::rectangle_t main_window_t::window_filling_region(const std::shared_ptr<layer_t>& layer)
      {
         geometry::rectangle_t region = convert(my_layered_canvas->geometry());
         region.x = region.y = 0;
         return region.apply(my_layered_canvas->get_local_transform().compose(layer->get_transform()).invert());
      }

      std::vector<std::shared_ptr<styled_mosaic_t>> main_window_t::get_avail_mosaics()
      {
         const auto& layers = get_avail_layers();
         std::vector<std::shared_ptr<styled_mosaic_t>> mosaics;
         for (const auto& layer : layers)
            if (const auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
               mosaics.push_back(mo_layer);
         return mosaics;
      }

      std::vector<std::shared_ptr<layer_t>> main_window_t::get_avail_layers()
      {
         return my_layered->get_layers();
      }

      void main_window_t::update_layered_transform()
      {
         geometry::rectangle_t bounds;
         for (const auto& mo_layer : get_avail_mosaics())
         {
            geometry::rectangle_t new_bounds = mo_layer->mosaic->tiling->bounds();
            if (new_bounds.is_invalid())
               continue;
            const auto& trf = mo_layer->get_transform();
            if (!trf.is_invalid())
               new_bounds = new_bounds.apply(trf);
            
            if (bounds.is_invalid())
               bounds = new_bounds;
            else
               bounds = bounds.combine(new_bounds);
         }

         if (bounds.is_invalid())
            return;

         geometry::rectangle_t region = convert(my_layered_canvas->geometry());
         double ratio = std::max(region.width / bounds.width, region.height / bounds.height);
         // Make it so we can see 9 instances (3x3) of the tiling or mosaic.
         my_layered->compose(transform_t::scale(ratio / 3.));
      }

      const geometry::edges_map_t& main_window_t::find_calculated_mosaic(mosaic_edges_map_cache_t& calc_mos, const std::shared_ptr<styled_mosaic_t>& styled_mosaic)
      {
         const auto& mosaic = styled_mosaic->mosaic;
         const auto& trf = styled_mosaic->get_transform();
         for (const auto& calculated : calc_mos)
         {
            if (calculated.trf == trf && *(calculated.mosaic) == *mosaic)
            {
               return calculated.edges_map;
            }
         }
         const auto edges_map = mosaic->construct(window_filling_region(styled_mosaic));
         calc_mos.emplace_back(mosaic, trf, std::move(edges_map));
         return calc_mos.back().edges_map;
      }

      void main_window_t::update_canvas_layers(const std::vector<std::shared_ptr<layer_t>>& layers)
      {
         // Optimize updating the layers by only calculating the map of a mosaic once
         // if multiple layers have identical mosaics.
         mosaic_edges_map_cache_t calc_mos;
         for (auto& layer : layers)
         {
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               const auto& calc_map = find_calculated_mosaic(calc_mos, mo_layer);
               mo_layer->style->set_map(calc_map, mo_layer->mosaic->tiling);
            }
         }
         my_layered_canvas->update();
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The layers UI call-backs.

      std::vector<std::shared_ptr<layer_t>> main_window_t::get_selected_layers()
      {
         return my_layer_list->get_selected_layers();
      }

      std::vector<std::shared_ptr<style_t>> main_window_t::get_selected_styles()
      {
         std::vector<std::shared_ptr<tiling_style::style_t>> selected;
         for (auto layer : my_layer_list->get_selected_layers())
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
         for (auto layer : my_layer_list->get_selected_layers())
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
         for (auto& mo_layer : get_avail_mosaics())
         {
            if (std::find(styles.begin(), styles.end(), mo_layer->style) != styles.end())
            {
               layers.emplace_back(mo_layer);
            }
         }
         return layers;
      }

      void main_window_t::update_layer_list()
      {
         my_layer_list->update_list_content();
      }

      void main_window_t::fill_layer_list()
      {
         my_layer_list->set_edited(my_layered->get_layers());
         my_styles_editor->set_edited(get_selected_styles());
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
         my_figure_list->set_edited(get_merged_avail_figures());
      }

      std::shared_ptr<figure_t> main_window_t::get_selected_figure()
      {
         return my_figure_list->get_selected_figure();
      }

      void main_window_t::fill_figure_editor(bool force_update)
      {
         if (const auto& figure = get_selected_figure())
         {
            my_figure_editor->set_edited(figure, force_update);
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
               style->style->set_map(edges_map_t(), nullptr);
            }
         }
      }

      void main_window_t::update_undo_redo_actions()
      {
         my_undo_action->setEnabled(my_undo_stack.has_undo());
         my_redo_action->setEnabled(my_undo_stack.has_redo());
      }

      void main_window_t::awaken_styled_mosaic(const std::any& data)
      {
         dak::ui::layered_t::layers_t layers = clone_layers(std::any_cast<const dak::ui::layered_t::layers_t&>(data));
         for (auto& layer : layers)
         {
            if (auto style_mosaic = std::dynamic_pointer_cast<styled_mosaic_t>(layer))
            {
               const auto& mosaic = style_mosaic->mosaic;
               const auto& style = style_mosaic->style;
               style->set_map(mosaic->construct(window_filling_region(style_mosaic)), mosaic->tiling);
            }
         }

         my_layered->set_layers(layers);

         fill_layer_list();

         const bool force_update = true;
         fill_figure_editor(force_update);

         update_canvas_layers(my_layered->get_layers());
      }

      void main_window_t::awaken_to_empty_canvas()
      {
         my_layered->set_layers({});

         fill_layer_list();

         const bool force_update = true;
         fill_figure_editor(force_update);

         update_canvas_layers(my_layered->get_layers());
      }

      void main_window_t::clear_undo_stack()
      {
         my_undo_stack.clear();
      }

      void main_window_t::commit_to_undo()
      {
         const dak::ui::layered_t::layers_t& layers = my_layered->get_layers();
         my_undo_stack.simple_commit(
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
         auto layers = my_layered->get_layers();
         const bool was_empty = (layers.size() <= 0);
         layers.emplace_back(mo_layer);
         my_layered->set_layers(layers);

         while (new_mosaic->tiling->count_fill_copies(window_filling_region(mo_layer)) > 20)
         {
            const point_t center = window_filling_region(mo_layer).center();
            my_layered->compose(transform_t::scale(2.));
         }

         mo_layer->update_style(window_filling_region(mo_layer));

         fill_layer_list();

         if (was_empty)
         {
            update_layered_transform();
            clear_undo_stack();
            // Note: when adding layers, allow undoing back to an empty my_layered_canvas.
            my_undo_stack.simple_commit({ 0, nullptr, [self=this](const std::any&) { self->awaken_to_empty_canvas(); } });
         }

         commit_to_undo();

         my_layered_canvas->update();
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The mosaic tool-bar buttons.

      void main_window_t::update_mosaic_map(const std::vector<std::shared_ptr<styled_mosaic_t>>& mosaics, const std::wstring& name)
      {
         std::vector<std::shared_ptr<layer_t>> layers(mosaics.begin(), mosaics.end());
         return update_mosaic_map(layers, name);
      }

      void main_window_t::update_mosaic_map(const std::vector<std::shared_ptr<layer_t>>& layers, const std::wstring& name)
      {
         my_layers_dock->setWindowTitle(QString::fromWCharArray(L::t(L"Layers for Mosaic: ")) + QString::fromWCharArray(name.c_str()));

         my_layered->set_layers(layers);
         if (layers.size() > 0)
            if (auto mo_layer = std::dynamic_pointer_cast<styled_mosaic_t>(my_layered->get_layers()[0]))
               update_layered_transform();

         fill_layer_list();
         commit_to_undo();

         update_canvas_layers(get_avail_layers());

         my_original_mosaic->make_similar(*my_layered);
      }

      /////////////////////////////////////////////////////////////////////////
      //
      // The my_layered_canvas manipulation tool-bar buttons.

      void main_window_t::update_canvas_mode()
      {
         auto forced_mode = dak::ui::transformer_t::interaction_mode_t::normal;
         if (my_translate_button->isChecked())
            forced_mode = dak::ui::transformer_t::interaction_mode_t::moving;
         else if (my_rotate_button->isChecked())
            forced_mode = dak::ui::transformer_t::interaction_mode_t::rotating;
         else if (my_scale_button->isChecked())
            forced_mode = dak::ui::transformer_t::interaction_mode_t::scaling;
         my_layered_canvas->transformer.forced_interaction_mode = forced_mode;
      }

   }
}
