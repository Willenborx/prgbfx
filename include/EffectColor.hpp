#ifndef PRGBFX_EffectColor_hpp
#define PRGBFX_EffectColor_hpp

#include <Color.hpp>

namespace prgbfx {
    using namespace prgb;

   /// @brief ColorMoveMode is used to cycle through colors of a color palette. Might need other information like the cycle time 
    enum ColorMoveMode { 
        CMV_None=0,         /// will always keep one color given by the constructor's offset parameter 
        CMV_Crossfade=1,    /// will fade continuously from one color to the next, so the color is always changing 
        CMV_Softswitch=2,   /// will switch "hard" to the new color when the delay time has expired 
        CMV_Switch=3        /// cycles through the colors. It uses the fade_ms parameter of the constructor to define how long the transition will take. Transition starts and delay_ms - fadetime_ms 
    };

    /**
     * @brief EffectColor ist an abstract class that is able to return a color depending on a time paramter passed to it. This may be used
     * to produce colors chaging over time. The function get_color() must be provided by implementations of this interface
     */
    class EffectColor {

        public:
            /// @brief An effect color is a base color for some effects / shapes. It uses an array of colors. a delay_ms parameter which defines a color change interval and an offest to indicate at which index
            ///        of the color palette it starts
            EffectColor()  { LOG("EffectColor: Create");}

            virtual ~EffectColor() { LOG("EffectColor: Destruct");}

            /// @brief  calculates the color at a given point in time. 
            /// @param delta_ms time stamp in ms
            /// @param idx which color in the array is calculated?
            /// @return color value
            virtual ColorValue get_color(TimeMS delta_ms, uint8_t idx=0) = 0;

    };

    /**
     * @brief get_color() always returns the static color
     */
    class EffectColorStatic : public EffectColor {
        //! the color 
        ColorValue color;

        public:
            EffectColorStatic(ColorValue color) 
                : EffectColor(), color(color) { LOG(" EffectColorStatic: Construct"); };
            
            inline ColorValue get_color(TimeMS delta_ms, uint8_t idx=0) { return color; };

    };

    /**
     * @brief Implements an abstract EffectColor base class which uses a @link ColorPalette @endlink to determine colors depending on the 
     *        time 
     */
    class EffectColorPaletteAbstract : public EffectColor {
            protected:
                //! a vector of colors is used to store all different colors. The derived class may use ths "pallete" for the effect
                const ColorPalette *colors = nullptr;

                /// @brief a period of time after which the derived class may take action
                TimeMS delay_ms;

                /// @brief offset in the color array to start with
                uint8_t offset;

                /// @brief the number of colors
                uint8_t size;

            public:
                EffectColorPaletteAbstract(const ColorPalette *colors, TimeMS delay_ms, uint8_t offset = 0) 
                    : EffectColor(), colors(colors), delay_ms(delay_ms), offset(offset), size((colors != nullptr) ? colors->size() : 0) {
                    LOG(" EffectColorPaletteAbstract: Construct");
                }

            /// @brief  replace the array of colors
            /// @param colors 
            void set_colors(const ColorPalette *colors) { this->colors = colors; };

            /// @brief  get_color is still abstract. Classes derived from this class can rely on the handling of the ColorPalette, the get_color
            ///         member function still needs to be overwritten
            /// @param delta_ms expired milliseconds
            /// @param idx an index value which e.g can deinfe the first color to be used.
            /// @return 
            virtual ColorValue get_color(TimeMS delta_ms, uint8_t idx=0) = 0;

    };


    /**
     * @brief Colors change through a palette of colors passed to the constructor, different modes define how to change between the colors.
     */
    class EffectColorMove : public EffectColorPaletteAbstract {
          
            /// @brief check @link ColorMoveMode
            ColorMoveMode mode;
            /// @brief duration of the color transition in COLORMODE_SOFTSWITCH. Ignored for all other modes
            TimeMS fade_ms;

        public:
            /// @brief depending on the @link ColorMoveMode @endlink colors will be cycled through after the defined delay in different ways.
            EffectColorMove(const ColorPalette *colors, TimeMS delay_ms, uint8_t offset = 0,ColorMoveMode mode = CMV_Switch, TimeMS fade_ms = 500)
                : EffectColorPaletteAbstract(colors, delay_ms, offset), mode(mode), fade_ms(fade_ms) {
                LOG(" EffectColorMove: Construct");
            }

            virtual ~EffectColorMove() {LOG(" EffectColorMove: Destruct");}

            /// calculates the color from the time 
            virtual ColorValue get_color(TimeMS delta_ms, uint8_t idx=0) {
                if (colors == nullptr) return 0;
                    
                const ColorPalette& colorv = *colors;
                if (size == 1) return (colorv[0]); // only one color -> nothing to calculate

                uint8_t colorno = (offset+idx+delta_ms/delay_ms) % size; // which color is the primary?
                
                uint32_t perthousand = 0;
                uint32_t timepos = (delta_ms%delay_ms); // determine which time of delay_ms has elapsed in this cycle

                uint32_t fadepointms = delay_ms-fade_ms;

                switch (mode) {
                    case CMV_Crossfade:
                        perthousand = 1000*timepos / delay_ms; // where are we between two colors?
                        return prgb::gradient(colorv[colorno],colorv[(colorno+1)%size], perthousand,1000);
                    case CMV_Softswitch:
                        perthousand = (timepos <= fadepointms) ? 0 : 1000*(timepos-fadepointms)/fade_ms;
                        return prgb::gradient(colorv[colorno],colorv[(colorno+1)%size], perthousand,1000);
                    case CMV_Switch:
                        return colorv[colorno];
                    default: // CMV_None
                        return colorv[(offset+idx)%size];
                    };
            }

            // resets the parameters to new values
            void reset(const ColorPalette *colors, TimeMS delay_ms, uint8_t offset, ColorMoveMode mode, TimeMS fade_ms = 500) {
                this->colors =colors;
                this->delay_ms = delay_ms;
                this->offset = offset;
                this->mode = mode;
                this->fade_ms = fade_ms;        
            }
    };



}

#endif