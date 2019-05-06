#include <dak/tiling_ui_qt/main_window.h>

#include <dak/utility/text.h>

#include <QtWidgets/qapplication.h>

#include "resource.h"

static HINSTANCE appInstance;

int main(int argc, char **argv)
{
   QScopedPointer<QApplication> app(new QApplication(argc, argv));

   dak::utility::L::initialize();

   dak::tiling_ui_qt::main_window_icons icons;

   icons.app_icon          = IDI_APP_ICON;

   icons.mosaic_previous   = IDB_MOSAIC_PREVIOUS;
   icons.mosaic_next       = IDB_MOSAIC_NEXT;

   icons.undo              = IDB_UNDO;
   icons.redo              = IDB_REDO;

   icons.mosaic_open       = IDB_MOSAIC_OPEN;
   icons.mosaic_save       = IDB_MOSAIC_SAVE;

   icons.export_img        = IDB_EXPORT_IMG;
   icons.export_svg        = IDB_EXPORT_SVG;

   icons.canvas_translate  = IDB_CANVAS_TRANSLATE;
   icons.canvas_rotate     = IDB_CANVAS_ROTATE;
   icons.canvas_zoom       = IDB_CANVAS_ZOOM;
   icons.canvas_redraw     = IDB_CANVAS_REDRAW;

   icons.layer_copy        = IDB_LAYER_COPY;
   icons.layer_add         = IDB_LAYER_ADD;
   icons.layer_delete      = IDB_LAYER_DELETE;
   icons.layer_move_up     = IDB_LAYER_MOVE_UP;
   icons.layer_move_down   = IDB_LAYER_MOVE_DOWN;

   icons.tiling_editor     = IDB_TILING_EDITOR;

   icons.tiling_new        = IDB_TILING_NEW;
   icons.tiling_open       = IDB_MOSAIC_OPEN;
   icons.tiling_save       = IDB_MOSAIC_SAVE;

   icons.add_polygon       = IDB_POLYGON_ADD;
   icons.draw_polygon      = IDB_POLYGON_DRAW;
   icons.copy_polygon      = IDB_POLYGON_COPY;
   icons.delete_polygon    = IDB_POLYGON_DELETE;
   icons.move_polygon      = IDB_POLYGON_MOVE;

   icons.toggle_inclusion  = IDB_POLYGON_INCLUDE;
   icons.exclude_all       = IDB_POLYGON_EXCLUDE_ALL;
   icons.fill_with_translation = IDB_POLYGON_FILL;
   icons.remove_excluded   = IDB_POLYGON_CLEAR_FILL;

   icons.clear_translation = IDB_POLYGON_TRANS_CLEAR;
   icons.draw_translation  = IDB_POLYGON_TRANSLATION;

   auto mainWindow = new dak::tiling_ui_qt::main_window(icons);

   mainWindow->resize(1000, 800);
   mainWindow->show();
   return app->exec();
}

int wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* lpCmdLine, int nCmdShow)
{
   appInstance = hInstance;
   return main(0, 0);
}

