#ifndef EffectGradient_hpp
#define EffectGradient_hpp

#include <Effect.hpp>
#include <PositionModifier.hpp>
#include <Color.hpp>
#include <Coordinates.hpp>
#include <Log.hpp>

namespace prgbfx {

    using namespace prgbfx;


    /**
     * @brief An effect which creates a gradient color related to one point: The higher the distance from this point is the
     *        closer the color is to the secondary color. This point can be moved using a @link PositionModifier @endlink. This creates
     *        some movement. Because this effect does calculations for each individual pixel, it's very computing intense.
     */
    class EffectGradient : public Effect {
    
            public:
            EffectGradient(LightArray& ar, RectArea& box, Point& pt_center, const PositionModifiers posmods, EffectColor& color, uint8_t brightness=100) 
                : Effect(ar), box(box), pt_center(pt_center), posmods(posmods), color(color), brightness(brightness) {
                    LOG("EffectGradient: Construct");
                    Dimension width = box.size.w << 7; // multiplier for more granularity - will be related to dist_max later on (using the same mulitplier)
                    Dimension height = box.size.h << 7;
                    dist_max = sqrt(width*width+height*height);
                }

            virtual void render_effect(TimeMS time_delta) {
                
                RectArea rect = RectArea(pt_center, Size(1,1));
                for (auto pos:posmods) {
                    rect = pos->calc_shape(time_delta,Point(rect.origin),Size(1,1));
                }

                ColorValue color_current = color.get_color(time_delta);
                ColorValue color_next = color.get_color(time_delta,2);
                if (!enabled) return;

                // Loop through all pixels
                for (int x=0; x < box.size.w; x++){
                    for (int y=0; y < box.size.h; y++) {

                        Dimension xdist = (rect.origin.x-x) << 7;
                        Dimension ydist = (rect.origin.y-y) << 7;

                        Dimension dist = sqrt(xdist*xdist+ydist*ydist);
                        ColorValue color_new = prgb::dim(prgb::gradient(color_current,color_next,dist,dist_max),brightness);

                        ar.set_pixel(Point(box.origin.x+x, box.origin.y+y), color_new, CMODE_Set);
                    }
                }

            }

        protected:
            RectArea &box;
            Point &pt_center;
            const PositionModifiers posmods;
            EffectColor& color;
            uint8_t brightness;
            Dimension dist_max;

    };
}


#endif