/**
 * @file ColorModifier.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief   ColorModifiers  are called during effect calculation. They can be used to modify colors for each frame. The abstract class provides the modify() member function which must be
 *          implemented in derived classes- A color is sent into the modify() function and it must return the modified color.
 * @version 0.5
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PRGB_COLORMODIFIER_HPP
#define PRGB_COLORMODIFIER_HPP

#include <Color.hpp>

namespace prgbfx {
    
    using namespace prgb;

    /// @brief Abstract base class. Effects can make use of this class to apply modifications to the color / brightness, the @link modify() @endlink 
    class ColorModifier {

        public:
            ColorModifier() { LOG("ColorModifier: Construct");}
            virtual ~ColorModifier() { LOG("ColorModifier: Destruct");}
            /// @brief  return the modified color
            /// @return the modified color
            virtual ColorValue modify(ColorValue color, TimeMS time_delta) = 0;

    };

    /// @brief A vector of ColorModifiers (for each effect, multiple Color Modifiers can be applied)
    typedef std::vector<ColorModifier *> ColorModifiers;

    /// @brief Strobo effect, requires on/off times which can not be shorter than one frame (so no real Strobo effect)
    class ColorModifierStrobo : public ColorModifier {
        uint16_t on_ms, off_ms;

        public: 
            ColorModifierStrobo(TimeMS on_ms, TimeMS off_ms) : ColorModifier() { LOG(" ColorModifierStrobo: Construct"); set_delay(on_ms,off_ms); };

            virtual ~ColorModifierStrobo() { LOG(" ColorModifierStrobo: Destruct");}

            virtual ColorValue modify(ColorValue color, TimeMS time_delta) {
                TimeMS interval = on_ms + off_ms;
                TimeMS pos = time_delta % interval;
                return (pos >= on_ms) ? 0 : color;
            }                

            void set_delay(TimeMS on_ms, TimeMS off_ms) {
                this->on_ms = on_ms;
                this->off_ms = off_ms;
            };
    };

    /// @brief Attenuates (< 100) or brightens (> 100) the color
    class ColorModifierStatic : public ColorModifier {
        uint16_t fade;

        public:
            ColorModifierStatic(uint16_t fade) : ColorModifier() { LOG(" ColorModifierStatic: Create"); this->fade = fade; }
            virtual ~ColorModifierStatic() {LOG(" ColorModifierStatic: Destruct"); }
            virtual ColorValue modify(ColorValue color, TimeMS time_delta) { return prgb::dim(color, fade); }
    
    };

};

#endif