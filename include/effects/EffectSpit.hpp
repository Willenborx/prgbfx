/**
 * @file EffectSpit.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef EFFECTSPIT_HPP
#define EFFECTSPIT_HPP

#include <EffectArrayAbstract.hpp>

namespace prgbfx
{
    using namespace prgb;

    class SpitDrop {
        
        public:
            TimeMS time_birth;
            TimeMS delay_y;
            ColorValue color;
            Point pt_current; 
            u_int8_t trail;
            int opacity;

            SpitDrop(TimeMS time_birth, TimeMS delay_y, ColorValue color, Point pt_current, uint8_t trail=3, int opacity=100) 
                : time_birth(time_birth), delay_y(delay_y), color(color), pt_current(pt_current), trail(trail), opacity(opacity) {
                
            };

    };

    class EffectSpit : public EffectArrayAbstract<SpitDrop> {

        // Initialized
        LoudnessBase& lb;
        SoundObserver& ob;

        RectArea& rect;
        EffectColor *color;
        uint8_t trail;

        // non-initialized

        TimeMS time_start;

        public:
            EffectSpit(LightArray* ar, LoudnessBase &lb, SoundObserver &ob, RectArea& rect, EffectColor* color, uint8_t trail=3) 
                : EffectArrayAbstract(ar), lb(lb), ob(ob), rect(rect), color(color), trail(trail) { 
                    time_start = ar->get_timebase().get_deltatime_ms();
                }
        
            void render_effect(TimeMS time_delta) {
                
                if (!enabled) return;

                // manage items
                if (false) {
                    
                    // add_item(...);

                }

                 // draw 
                 for_each([this, time_delta](SpitDrop& item){
                    return(false);

                 });
            }

    };
    
} // namespace prgbfx

#endif