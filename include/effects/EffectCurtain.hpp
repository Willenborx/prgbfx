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
#ifndef EFFECTCURTAIN_HPP
#define EFFECTCURTAIN_HPP

#include <EffectArrayAbstract.hpp>

namespace prgbfx
{
    using namespace prgb;

    class CurtainThread {
        
        public:
            TimeMS time_birth;
            TimeMS delay_y;
            ColorValue color;
            Point pt_current; 

            CurtainThread(TimeMS time_birth, TimeMS delay_y, ColorValue color, Point pt_current) 
                : time_birth(time_birth), delay_y(delay_y), color(color), pt_current(pt_current) {
                
            };

    };

    class EffectCurtain : public EffectArrayAbstract<CurtainThread> {

        // Initialized
        LoudnessBase& lb;
        SoundObserver& ob;

        RectArea& rect;
        EffectColor *color;
        TimeMS delay_x;
        TimeMS delay_y;

        // non-initialized

        TimeMS time_start;
        Dimension x_last = 5555;

        public:
            EffectCurtain(LightArray* ar, LoudnessBase &lb, SoundObserver &ob, RectArea& rect, EffectColor* color, TimeMS delay_x=100, TimeMS delay_y=100) 
                : EffectArrayAbstract(ar), lb(lb), ob(ob), rect(rect), color(color), delay_x(delay_x), delay_y(delay_y) { 
                    time_start = ar->get_timebase().get_deltatime_ms();
                }
        
            void render_effect(TimeMS time_delta) {
                
                Coordinate x = ((time_delta - time_start)/delay_x) % rect.size.w;

                if (x != x_last) {
                    x_last = x;
                    add_item(CurtainThread(time_delta,rand()%25+30,color->get_color(time_delta),Point(x,rect.size.h)));

                }

                // manage items
                // if ... add_item()


                 // draw 
                 for_each([this, time_delta](CurtainThread& item){

                    item.pt_current.y = rect.size.h-(time_delta-item.time_birth)/item.delay_y-1;
                    ar->set_pixel(item.pt_current,item.color,CMODE_Set);
                    return (item.pt_current.y > 0);

                 });
            }

    };
    
} // namespace prgbfx

#endif