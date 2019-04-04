#include <dak/ui_qt/canvas.h>

#include <QtGui/qpainter.h>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A QWidget that brings together a transformer and widget emitter
      // with some transformable to be provided by a sub-class.

      canvas::canvas(QWidget* parent)
      : QWidget(parent)
      , transformer([this](dak::ui::transformer& trfer) { update(); })
      , emi(*this)
      {
         emi.event_receivers.push_back(&transformer);
         setContentsMargins(0, 0, 0, 0);
      }

      canvas::canvas(QWidget* parent, ui::transformable& trfable)
      : canvas(parent)
      {
         transformer.manipulated = &trfable;
         emi.event_receivers.push_back(&transformer);
      }

      void canvas::paintEvent(QPaintEvent * pe)
      {
         QPainter painter(this);
         painter.setRenderHint(QPainter::RenderHint::Antialiasing);
         painter.setRenderHint(QPainter::RenderHint::TextAntialiasing);
         painter.setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);

         paint(painter);
      }

      void canvas::paint(QPainter& painter)
      {
         dak::ui_qt::painter_drawing trfer_drawing(painter);
         transformer.draw(trfer_drawing);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
