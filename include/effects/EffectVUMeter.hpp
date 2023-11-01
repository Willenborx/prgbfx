/**
 * @file EffectVUMeter.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief A simple VU effect displaying the measurings of the 6 bands used by \link LoudnessBase \link
 * @version 0.5
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <Effect.hpp>
#include <EffectColor.hpp>

#include <LoudnessBase.hpp>
#include <TimeBase.hpp>

using namespace prgb;
using namespace prgbfx;

const uint32_t relationTime = 60000;

class EffectVUMeter : public Effect {
    Softener<Loudness> bandsoft[6], bandrelation[6], mxsoft = Softener<Loudness>(2000);
    Loudness bandval[2][6];
    uint8_t current = 0;
    Dimension distance, offset;
    LoudnessBase& lb;
    RectArea box;
    EffectColor& color;

    public:
        EffectVUMeter(LightArray& ar, LoudnessBase& lb, RectArea box, EffectColor& color, int VUdelay=200) : Effect(ar), lb(lb), box(box), color(color) {
            LOG(" EffectVUMeter: Construct");
            for (int i = 0; i < 6; i++) { 
                bandsoft[i].set_delay(VUdelay); 
                bandrelation[i].set_delay(relationTime);
            }
            Dimension width = box.size.w;
            distance = (width / 6);
            offset = (width % 6) / 2;

        }

        virtual ~EffectVUMeter() { LOG(" EffectVUMeter: Destruct");}

        virtual void render_effect(TimeMS time_delta) {
            Loudness maxld = 0;
            for (int i = 0; i < 6; i++) {
                
                bandval[current][i] = lb.get_freq_band(i);
                if (bandval[current][i] > maxld) maxld = bandval[current][i]; 
            }
            maxld = 4000; // #ugly

            if (!enabled) return;

            for (int i=0; i<6; i++) {
                //Loudness sftmax = mxsoft.value(time_delta,maxld);
                Loudness sftlevel = normalize<Loudness,100>(bandsoft[i].value(time_delta,bandval[current][i]),maxld,box.size.h);
                
                ar.fill_rect(
                        box.origin.x+distance*i+offset,
                        box.origin.y,
                        distance-1,
                        sftlevel,
                        color.get_color(time_delta),CMODE_Set);
                

            }
            
            current = (current == 0) ? 1 : 0;

        }

      
};
