#pragma once

#ifndef DAK_UI_TRANSFORMER_H
#define DAK_UI_TRANSFORMER_H

#include <dak/ui/transformable.h>
#include <dak/ui/modifiers.h>
#include <dak/ui/mouse.h>
#include <dak/ui/drawing.h>

#include <dak/geometry/constants.h>

#include <functional>

namespace dak
{
   namespace ui
   {
      using geometry::PI;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Transformer has all kinds of useful built-in interactions
      // for changing the transform of something.

      class transformer : public mouse::receiver
      {
      public:
         // What is being transformed and where it is drawn.
         transformable* manipulated;
         std::function<void(transformer&)> draw_callback;

         // Mode used to track how the mouse is handled: normally, move, rotate, scale.
         enum class interaction_mode
         {
            normal   = 0,
            moving   = 1,
            rotating = 2,
            scaling  = 3,

            lowest   = 0,
            highest  = 3,
         };

         // Force a particular interaction mode.
         interaction_mode forced__interaction_mode = interaction_mode::normal;

         // Creation.
         transformer(std::function<void(transformer&)> draw_callback);
         transformer(transformable& tr, std::function<void(transformer&)> draw_callback);

         void draw(drawing& drw);

         // Mouse event handling from mouse::receiver.
         void mouse_clicked(const mouse::event& me) override;
         void mouse_entered(const mouse::event& me) override;
         void mouse_exited(const mouse::event& me) override;
         void mouse_pressed(const mouse::event& me) override;
         void mouse_released(const mouse::event& me) override;
         void mouse_moved(const mouse::event& me) override;
         void mouse_dragged(const mouse::event& me) override;
         void mouse_wheel(const mouse::event& me) override;

      private:
         // Interaction modes.
         interaction_mode get_wheel_interaction_mode(const mouse::event& me);
         interaction_mode get_button_interaction_mode(const mouse::event& me);

         // Drawing current interaction.
         void draw_moving(drawing& drawing);
         void draw_rotating(drawing& drawing);
         void draw_scaling(drawing& drawing);

         // Internal data handling.
         void clear_interaction_data();
         void update_tracked_position(const mouse::event& me);

         // Mouse interactions.
         void start_interaction(const mouse::event& me);
         void end_interaction();

         void common_move(const point& from, const point& to);
         void common_rotate(const point& from, const point& to);
         void common_scale(const point& from, const point& to);

         void drag_move(const mouse::event& me);
         void drag_rotate(const mouse::event& me);
         void drag_scale(const mouse::event& me);

         void wheel_move(const mouse::event& me);
         void wheel_rotate(const mouse::event& me);
         void wheel_scale(const mouse::event& me);

         double calculate_scaling(const point& p);
         point screen_to_transformable(const point& screen_point) const;

         // User input tracking..
         point last_point;
         point tracked_point;
         point initial_point;
         bool has_moved_enough_to_interact = false;
         interaction_mode _interaction_mode = interaction_mode::normal;

         // Current transformation during one interaction.
         double interaction_rotating_angle = 0;
         double interaction_scaling_factor = 0;
         point  interaction_moving_delta = point::origin();

         // Size of drawings showing feedback for the interaction.
         static const int manipSizePercent = 80;
         static double get_feedback_size(const rect& r);

         // Color of feedback drawings.
         static constexpr color draw_color() { return color(225, 190, 50, 190); }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

