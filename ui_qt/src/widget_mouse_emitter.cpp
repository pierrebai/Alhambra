#include <dak/ui_qt/widget_mouse_emitter.h>

#include <QtGui/qevent.h>

namespace dak
{
   namespace ui_qt
   {
      namespace mouse
      {
         ////////////////////////////////////////////////////////////////////////////
         //
         // Helpers.

         namespace
         {
            buttons translate(const Qt::MouseButtons b)
            {
               buttons tb = buttons::none;
               if (b & Qt::MouseButton::LeftButton)
                  tb = tb | buttons::one;
               if (b & Qt::MouseButton::MiddleButton)
                  tb = tb | buttons::three;
               if (b & Qt::MouseButton::RightButton)
                  tb = tb | buttons::two;
               return tb;
            }

            modifiers translate(const Qt::KeyboardModifiers m)
            {
               modifiers tm = modifiers::none;
               if (m & Qt::Modifier::SHIFT)
                  tm = tm | modifiers::shift;
               if (m & Qt::Modifier::CTRL)
                  tm = tm | modifiers::control;
               if (m & Qt::Modifier::ALT)
                  tm = tm | modifiers::alt;
               return tm;
            }

            ui::mouse::event translate(QMouseEvent* qme)
            {
               if (!qme)
                  return ui::mouse::event(point(), buttons::none, modifiers::none, 0);

               qme->accept();

               return ui::mouse::event(point(qme->x(), qme->y()),
                  translate(qme->buttons()), translate(qme->modifiers()), 0);
            }

            ui::mouse::event translate(QWheelEvent* qwe)
            {
               if (!qwe)
                  return ui::mouse::event(point(), buttons::none, modifiers::none, 0);

               qwe->accept();

               return ui::mouse::event(point(qwe->x(), qwe->y()),
                  translate(qwe->buttons()), translate(qwe->modifiers()), qwe->delta());
            }
         }

         ////////////////////////////////////////////////////////////////////////////
         //
         // QtWidget mouse emitter.

         widget_emitter::widget_emitter(QWidget& w)
         : widget(w)
         {
            widget.installEventFilter(this);
            widget.setMouseTracking(true);
         }

         widget_emitter::~widget_emitter()
         {
            widget.removeEventFilter(this);
         }

         bool widget_emitter::eventFilter(QObject* obj, QEvent* ev)
         {
            switch (ev->type())
            {
               case QEvent::Type::Enter:
                  mouse_entered(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
               case QEvent::Type::Leave:
                  mouse_exited(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
               case QEvent::Type::MouseButtonPress:
                  mouse_pressed(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
               case QEvent::Type::MouseButtonRelease:
                  mouse_released(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
               case QEvent::Type::MouseMove:
               {
                  auto me = translate(dynamic_cast<QMouseEvent *>(ev));
                  if (me.has_buttons(buttons::all))
                     mouse_dragged(me);
                  else
                     mouse_moved(me);
                  return true;
               }
               case QEvent::Type::DragEnter:
                  mouse_entered(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
               case QEvent::Type::DragLeave:
                  mouse_exited(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
               case QEvent::Type::DragMove:
                  mouse_dragged(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
               case QEvent::Type::Wheel:
                  mouse_wheel(translate(dynamic_cast<QWheelEvent *>(ev)));
                  return true;
               case QEvent::Type::MouseButtonDblClick:
                  mouse_clicked(translate(dynamic_cast<QMouseEvent *>(ev)));
                  return true;
            }

            return false;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
