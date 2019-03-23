#pragma once

#ifndef DAK_UI_MOUSE_H
#define DAK_UI_MOUSE_H

#include <dak/ui/modifiers.h>

#include <dak/geometry/point.h>

#include <vector>

namespace dak
{
   namespace ui
   {
      using geometry::point;

      namespace mouse
      {
         ////////////////////////////////////////////////////////////////////////////
         //
         // Mouse buttons. Support three buttons.

         enum class buttons
         {
            none  = 0,
            one   = 1,
            two   = 2,
            three = 4,
            all   = 7,
         };

         inline buttons operator&(buttons a, buttons b) { return buttons(int(a) & int(b)); }
         inline buttons operator|(buttons a, buttons b) { return buttons(int(a) | int(b)); }
         inline buttons operator^(buttons a, buttons b) { return buttons(int(a) ^ int(b)); }

         ////////////////////////////////////////////////////////////////////////////
         //
         // Mouse input event.

         class event
         {
         public:
            // On-screen position relative to the widget.
            point position;

            // Mouse buttons being pressed down.
            mouse::buttons buttons;

            // Keyboard modifiers being held down.
            ui::modifiers modifiers;

            // Amount of mouse-wheel scrolling.
            int scroll_amount = 0;

            // Verify if the given buttons are pressed down.
            // Return true if any are pressed.
            bool has_buttons(mouse::buttons a) const { return (int(buttons) & int(a)) != 0; }

            // Verify if the given modifiers are held down.
            // Return true if any are pressed.
            bool has_modifiers(ui::modifiers a) const { return (int(modifiers) & int(a)) != 0; }

            // Create an event.
            event(const point& position, mouse::buttons buttons, ui::modifiers modifiers, int scroll_amount)
            : position(position), buttons(buttons), modifiers(modifiers), scroll_amount(scroll_amount)
            {
            }
         };

         ////////////////////////////////////////////////////////////////////////////
         //
         // Interface needed to receive mouse events.

         class receiver
         {
         public:
            virtual ~receiver() = 0 { }

            // Various types of event that can be received.
            virtual void mouse_clicked(const event&) = 0;
            virtual void mouse_entered(const event&) = 0;
            virtual void mouse_exited(const event&) = 0;
            virtual void mouse_pressed(const event&) = 0;
            virtual void mouse_released(const event&) = 0;
            virtual void mouse_moved(const event&) = 0;
            virtual void mouse_dragged(const event&) = 0;
            virtual void mouse_wheel(const event&) = 0;
         };

         ////////////////////////////////////////////////////////////////////////////
         //
         // Generator of mouse events.

         class emitter
         {
         public:
            // Note: make sure to remove receivers before they are destroyed.
            // Note: not thread-safe. Use external mutex if needed.
            std::vector<receiver *> event_receivers;

            // Send the given event to all registered receivers.
            void mouse_clicked(const event& e);
            void mouse_entered(const event& e);
            void mouse_exited(const event& e);
            void mouse_pressed(const event& e);
            void mouse_released(const event& e);
            void mouse_moved(const event& e);
            void mouse_dragged(const event& e);
            void mouse_wheel(const event& e);

         private:
            // Member function type of receivers.
            typedef void (receiver::*event_receiver)(const event&);

            // Send the event to the specified receiver member function.
            void send(const event& e, event_receiver er);
         };

      }
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
