/**
 * @file EffectHello.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Hello Effecft demonstration
 * @version 0.6
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef EFFECTHELLO_HPP
#define EFFECTHELLO_HPP

#include <Effect.hpp>


namespace prgbfx {

    using namespace prgb;

    /**
     * @brief An effect for demonstration purposes. Implements a very simple @link render_effect() @endlink funtion to blink the entire
     *        matrix every second
     */
    class EffectHello : public Effect {

        RectArea canvas = RectArea(Point(0,0), ar->get_geometry().get_canvas_size());

        public:

            EffectHello(LightArray* ar) : Effect(ar) { }

            /**
             * @brief render a simple effect
             * 
             * @param delta_time the time expired since reset
             */
            virtual void render_effect(TimeMS delta_time) {
                if (!enabled) return true;
                int8_t c = 64*((delta_time/1000) % 2);
                ar->fill_rect(canvas,RGB(c,c,c),CMODE_Set);
            };

    };
}

#endif
