/**
 * @file EffectLoudnessLines.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Lines that change color depending on the measured loudness
 * @version 0.6
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
  * 
 */

#ifndef EffectLoudnessLines_HPP
#define EffectLoudnessLines_HPP

#include <Effect.hpp>
#include <Log.hpp>

namespace prgbfx
{
    
    using namespace prgb;
    /// @brief Implements an @linkeffect that creates and shifts lines with brightness related to the current loudness
    class EffectLoudnessLines : public Effect {
        public:
            EffectLoudnessLines(LightArray* ar, LoudnessBase& lb, LoudnessMode ldmode, RectArea& box, Direction direction, TimeMS delay_ms, 
                                EffectColor* color, EffectColor* color_bg,ColorModifiers colmods={}, ColorModifiers colbgmods={}) 
                : Effect(ar), lb(lb), ldmode(ldmode), box(box), direction(direction), delay_ms(delay_ms), color(color), color_bg(color_bg),colmods(colmods), colbgmods(colbgmods) {
                LOG(" EffectLoudnessLines: Construct");
                Dimension extent = get_extent();
                linecolors.reserve(extent);
                for (int i=0; i < extent; i++) {
                    linecolors[i] = color_bg->get_color(0,0);
                    for (auto cmod : colbgmods) { 
                        linecolors[i] = cmod->modify(linecolors[i],0);
                    }

                }
            };

            virtual ~EffectLoudnessLines() {LOG(" EffectLoudnessLines: Destruct");}
            
            virtual void render_effect(TimeMS time_delta) {
                if (!enabled) return;
                Dimension extent = get_extent();
                Loudness ld_now = lb.get_loudness(LD_Band_Bass);
                int16_t fadeval = (100*ld_now) / softfade.value(time_delta,ld_now) ;

                TimeMS position = time_delta % (delay_ms*extent); // cut to time window required to fill all lines

                int16_t idx = position / delay_ms;

                ColorValue color_bgnew = color_bg->get_color(time_delta);
                for (auto cmod : colbgmods) { 
                    color_bgnew = cmod->modify(color_bgnew,time_delta);
                }
                ColorValue color_new = prgb::gradient(color_bgnew,color->get_color(time_delta),fadeval,100);
                for (auto cmod : colmods) { 
                    color_new = cmod->modify(color_new,time_delta); 
                }

                linecolors[idx] = color_new;

                for (int i =0; i < get_extent(); i++) {
                    
                    for (int j=0; j<getLineLength(); j++) {
                        switch(direction) {
                            case DIR_Right:
                                ar->set_pixel(Point(box.origin.x+i,box.origin.y+j),linecolors[(idx-i+extent)%extent],CMODE_Set);
                                break;
                            case DIR_Left:
                                ar->set_pixel(Point(box.origin.x+box.size.w-i-1,box.origin.y+j),linecolors[(idx-i+extent)%extent],CMODE_Set);
                                break;
                            case DIR_Down:
                                ar->set_pixel(Point(box.origin.x+j,box.origin.y+i),linecolors[(idx-i+extent)%extent],CMODE_Set);
                                break;
                            case DIR_Up:
                                ar->set_pixel(Point(box.origin.x+j,box.origin.y+box.size.h-i-1),linecolors[(idx-i+extent)%extent],CMODE_Set);
                                break;
                            
                        }
                        
                    }
                }
            }

            Dimension get_extent() { return (direction == DIR_Left || direction == DIR_Right) ? box.size.w : box.size.h; }
            Dimension getLineLength() {return (direction == DIR_Left || direction == DIR_Right) ? box.size.h : box.size.w; } 
        
        protected:
            LoudnessBase& lb;
            LoudnessMode ldmode;
            RectArea& box;
            Direction direction;
            TimeMS delay_ms;
            ColorPalette linecolors;
            EffectColor* color; 
            EffectColor* color_bg;
            const ColorModifiers colmods;
            const ColorModifiers colbgmods;

            Softener<uint16_t> softfade = Softener<uint16_t>(100);
    };
}
#endif