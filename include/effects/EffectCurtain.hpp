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
            u_int8_t trail;
            int opacity;

            CurtainThread(TimeMS time_birth, TimeMS delay_y, ColorValue color, Point pt_current, uint8_t trail=3, int opacity=100) 
                : time_birth(time_birth), delay_y(delay_y), color(color), pt_current(pt_current), trail(trail), opacity(opacity) {
                
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
        uint8_t trail;

        // non-initialized

        TimeMS time_start;
        Dimension x_last = 5555;

        public:
            EffectCurtain(LightArray* ar, LoudnessBase &lb, SoundObserver &ob, RectArea& rect, EffectColor* color, TimeMS delay_x=100, TimeMS delay_y=100, uint8_t trail=3) 
                : EffectArrayAbstract(ar), lb(lb), ob(ob), rect(rect), color(color), delay_x(delay_x), delay_y(delay_y),trail(trail) { 
                    time_start = ar->get_timebase().get_deltatime_ms();
                }
        
            void render_effect(TimeMS time_delta) {
                
                if (!enabled) return;

                Coordinate x = ((time_delta - time_start)/delay_x) % rect.size.w;

                // manage items
                if (x != x_last) {
                    x_last = x;
                    add_item(CurtainThread(time_delta,rand()%25+30,prgb::dim(color->get_color(time_delta),200),Point(x,rect.size.h),trail,ob.get_ld_0_100()));

                }

                 // draw 
                 for_each([this, time_delta](CurtainThread& item){

                    item.pt_current.y = rect.size.h-(time_delta-item.time_birth)/item.delay_y-1;
                    for (int i=0; i < item.trail;i++)
                    {
                        int opacity = (100*(item.trail-i-1))/item.trail;
                        //int opacity = 100;
                        if ((item.pt_current.y+i < rect.size.h) && item.pt_current.y+i >= 0) ar->set_pixel(item.pt_current.translate(rect.origin).translate(0,i),item.color,CMODE_Transparent,item.opacity*opacity/100);
                    }
                    return (item.pt_current.y+item.trail > 0);

                 });
            }

    };
    
} // namespace prgbfx

#endif