/**
 * @file EffectDots.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief 
 * @version 0.1
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef EFFECDOTS_HPP
#define EFFECDOTS_HPP

#include <EffectArrayAbstract.hpp>

namespace prgbfx
{
    using namespace prgb;

    class ParticleDot {
        
        public:

            Circle circle;
            TimeMS item_birth;
            TimeMS item_lifetime;

            bool kill = false;

            ParticleDot(LightArray* ar,  CircleInit& ci, TimeMS item_birth, TimeMS item_lifetime) : circle(ar, ci), item_birth(item_birth), item_lifetime(item_lifetime) {};
            ~ParticleDot() { }
             ParticleDot(const ParticleDot& t) : circle(t.circle), item_birth(t.item_birth), item_lifetime(t.item_lifetime) {};
            

    };

    class EffectDots : public EffectArrayAbstract<ParticleDot> {

        LoudnessBase& lb;
        SoundObserver& ob;
        EffectColor* color, *color2;
        TimeMS time;
        TimeMS last_triggered = 0;

        ColorModifierStatic cm_static = ColorModifierStatic(150);
        EffectColorStatic clr_static = EffectColorStatic(RGB(255,255,255));

        public:
            EffectDots(LightArray* ar, LoudnessBase &lb, SoundObserver &ob, EffectColor* color, EffectColor* color2) : EffectArrayAbstract(ar), lb(lb), ob(ob), color(color), color2(color2), time(ar->get_timebase().get_deltatime_ms()) { }
        
            void render_effect(TimeMS time_delta) {

                if (!enabled) return;

                if (check_trigger(time_delta)) {

                    Size size_canvas = ar->get_geometry().get_canvas_size();
                    Dimension size_dot = 2*(1+rand()%4) + 1;

                    int8_t cp = rand()%3;

                    CircleInit ci = CircleInit({
                                .box=RectArea(rand()%size_canvas.w, rand()%size_canvas.h, size_dot, size_dot),
                                .posmods = {},
                                .color= (cp==0) ? color : ((cp==1) ? color2 : &clr_static),
                                .mode = CMODE_Transparent,
                                .colmods = {&cm_static},
                                .opacity = 100
                            });

                    add_item(ParticleDot(ar,ci,time_delta,100+70*size_dot+rand()%500));
                }

                 for_each([this, time_delta](ParticleDot& item){
                        TimeMS current_lifetime = time_delta - item.item_birth;
                        if (current_lifetime <= item.item_lifetime) { 
                            item.circle.drawmod(time_delta);                        
                            item.circle.set_opacity(100-100*current_lifetime/item.item_lifetime);
                            return true;
                        } else {
                           return false;
                        }
                 });
            }



        private:
            bool check_trigger(TimeMS time_delta) {
                //if ((last_triggered) == 0 || ((time_delta - last_triggered) > 250)) {
                if (ob.is_flag_set(SoundObserver::SO_DynamicPeak) && (time_delta - last_triggered > 10)) {
                    last_triggered = time_delta;
                    return true;
                } else return false;
            }
        

    };
    
} // namespace prgbfx
#endif