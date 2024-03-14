/**
 * @file EffectFountain.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief 
 * @version 0.6
* @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
  * 
 */

#ifndef EffectFountain_hpp
#define EffectFountain_hpp

#include <effects/EffectArrayAbstract.hpp>

#include <functional>
#include <Sine.hpp>

#include <LoudnessBase.hpp>

namespace prgbfx {

    using namespace prgb;

    /**
     * @brief This struct stores the information about the spawned "particles"
     * 
     */
    struct FountainParticle {
        Point position;
        int speed_x, speed_y; // speed in pxiels per second
        TimeMS time_spawn;
        ColorValue color;
    };

    /**
     * @brief An effect that simulates a "fountain" spawning "particles" which obey gravity
     *
     */
    class EffectFountain : public EffectArrayAbstract<FountainParticle> {

        TimeMS time_last_spawn = 0;
        TimeMS time_spawn_delay;
        LoudnessBase &lb;
        EffectColor* color;

        RectArea box = ar->get_geometry().get_canvas();
        Softener<Loudness> ldsoft = Softener<Loudness>(1000);

        public:
            EffectFountain(LightArray* ar, TimeMS time_spawn_delay, LoudnessBase &lb, EffectColor* color) : EffectArrayAbstract(ar), time_spawn_delay(time_spawn_delay), lb(lb), color(color) { }

            void render_effect(TimeMS time_delta) {

                // First check if new items need to be spawned
                TimeMS delta = time_delta - time_start;

                if (time_last_spawn == 0) time_last_spawn = delta;

                if (((delta-time_last_spawn) > time_spawn_delay) && enabled) {
                    Loudness ldraw = lb.get_loudness(LD_Realtime);
                    Loudness ldsoftval = ldsoft.value(delta,ldraw);
                    //if (lb.get_loudness_db(LD_Realtime) >= (lb.get_loudness_db(LD_environment) + 6.0)) {
                    if (ldsoftval == ldsoft.get_value_peak()) {
                        time_last_spawn = delta;
                        int xspeed = sine[(delta*20/1000)%90]/5;
                        int yspeed = (int) sqrt(45*45-xspeed*xspeed);
                        additem(FountainParticle({
                                    Point((xspeed > 0) ? box.size.w/8 : 7*box.size.w/8,1),
                                    xspeed,
                                    yspeed,
                                    delta,
                                    color->get_color(delta)})
                        );
                    }

                }

                // now calculate each Particle

                const int gravity = 35;

                for_each([this,delta](FountainParticle& item){

                        TimeMS delta_item = delta-item.time_spawn;

                        Dimension x = item.position.x + item.speed_x*delta_item/1000;
                        Dimension y = item.position.y + (item.speed_y-gravity*delta_item/1000)*delta_item/1000;
                        int16_t intensity = 255-delta_item/7;
                        int8_t opacity = (intensity > 100) ? 100 : intensity;
                        ColorValue faded = prgb::dim(item.color,75);
                        if ((x < 0) || (x >= box.size.w) || (y < 0) || (intensity-20 < 0)) {
                            return false;
                        }
                        if ((y < box.size.h)) {
                            ar->set_pixel(Point(x,y),item.color,CMODE_Transparent,opacity);
                            ar->set_pixel(Point(x-1,y),faded,CMODE_Transparent,opacity);
                            ar->set_pixel(Point(x+1,y),faded,CMODE_Transparent,opacity);
                            if (y+1 < box.size.h) ar->set_pixel(Point(x,y+1),faded,CMODE_Transparent,opacity);
                            if (y > 0) ar->set_pixel(Point(x,y-1),faded,CMODE_Transparent,opacity);
                        }
                        return true;
                    });
            }


    };
};
#endif