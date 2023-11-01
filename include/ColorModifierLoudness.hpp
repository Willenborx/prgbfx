/**
 * @file ColorModifierLoudness.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Modifies the brightness of a color according to the loudness
 * @version 0.5
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PRGB_COLORMODIFIERHPP
#define PRGB_COLORMODIFIERHPP

#include <ColorModifier.hpp>
#include <Color.hpp>
#include <LoudnessBase.hpp>
#include <TimeBase.hpp>

namespace prgbfx {

    using namespace prgbfx;

    /// @brief Mofifies the brightness depending on the loudness    
    class ColorModifierLoudness : public ColorModifier {

        public:
            /// @brief modifies the brightness depending on the loudness of the sound
            /// @param lb Loudness base implementation
            /// @param ld_mode loudness mode
            /// @param fade_ms delay-to-zero parameter, softens the decrease of the value 
            ColorModifierLoudness(LoudnessBase& lb, LoudnessMode ld_mode, TimeMS fade_ms=250) 
                : ColorModifier(),lb(lb), ld_mode(ld_mode), fade_ms(fade_ms)  {
                    LOG(" ColorModifierLoudness: Create"); 
                };

            virtual ~ColorModifierLoudness() { LOG(" ColorModifierLoudness: Destruct"); }
            
            virtual ColorValue modify(ColorValue color, TimeMS time_delta) {

                Loudness loud = lb.get_loudness(ld_mode);
                Loudness ref = sft_reference.value(time_delta,loud);

                loud = sft_level.normalized(time_delta,loud,ref,100);
                
                return prgb::dim(color,loud);
            }
        
        protected:
            LoudnessBase& lb;
            LoudnessMode ld_mode;
            uint16_t sensitivity;
            TimeMS fade_ms;

            Loudness    ld_last = 0, 
                        ld_peek = 0;
            
            TimeMS  time_peak = 0,
                    time_last = 0;

            Softener<Loudness> sft_reference = Softener<Loudness>(60000);
            Softener<Loudness> sft_level = Softener<Loudness>(fade_ms);
        
    };

}
#endif