#include <dak/ui/mouse.h>

namespace dak
{
   namespace ui
   {
      namespace mouse
      {
         void emitter::mouse_clicked(const event& e)
         {
            send(e, &receiver::mouse_clicked);
         }

         void emitter::mouse_entered(const event& e)
         {
            send(e, &receiver::mouse_entered);
         }

         void emitter::mouse_exited(const event& e)
         {
            send(e, &receiver::mouse_exited);
         }

         void emitter::mouse_pressed(const event& e)
         {
            send(e, &receiver::mouse_pressed);
         }

         void emitter::mouse_released(const event& e)
         {
            send(e, &receiver::mouse_released);
         }

         void emitter::mouse_moved(const event& e)
         {
            send(e, &receiver::mouse_moved);
         }

         void emitter::mouse_dragged(const event& e)
         {
            send(e, &receiver::mouse_dragged);
         }

         void emitter::mouse_wheel(const event& e)
         {
            send(e, &receiver::mouse_wheel);
         }

         void emitter::send(const event& e, event_receiver er)
         {
            for (receiver* rec : event_receivers)
               (rec->*er)(e);
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
