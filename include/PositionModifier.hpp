/**
 * @file PositionModifier.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Allows to modify the position / size of a shape depending on a timestamp
 * @version 0.6
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
  * 
 */

#ifndef PRGB_POSITIONMODIFIER_HPP
#define PRGB_POSITIONMODIFIER_HPP

#include <TimeBase.hpp>
#include <LoudnessBase.hpp>
#include <Limiter.hpp>

namespace prgbfx {

    using namespace prgb;

    /**
     * @brief Effect that use the PositionModifier can use modifiers derived from this class to
     *        modify position and/or size of the rectangular area used by an effect.
     */
    class PositionModifier {
        public:
            PositionModifier(LightArray* ar) : ar(ar) { LOG("PositionModifier: Construct");};
            virtual RectArea calc_shape(TimeMS time_delta, Point origin, Size size) { 
                return { 
                    {origin.x, origin.y},
                    {size.w, size.h}
                };
            };
            virtual ~PositionModifier() {LOG("PositionModifier: Destruct");};
        protected:
            LightArray* ar;
    };

    typedef PositionModifier PositionModifierStatic;
    typedef std::vector<PositionModifier *> PositionModifiers;

/*    class PositionModifierLinearMotion : public PositionModifier {
        
        public:
            PositionModifierLinearMotion(LightArray* ar, RectArea box, TimeMS delay_x_ms, TimeMS delay_y_ms):  PositionModifier(ar),  delay_x_ms(delay_x_ms), delay_y_ms(delay_y_ms), box(box) { LOG(" PositionModifierLinearMotion: Construct");};
            virtual RectArea calc_shape(TimeMS time_delta, Point origin, Size size) {
                Point origin_mod = origin;
                Size size_mod = size;

                if (delay_x_ms != 0) {
                    Coordinate xmax = box.size.w;
                    int16_t step = ((TimeMS) time_delta) / delay_x_ms;
                    origin_mod.set_x((step+origin.x+xmax) % xmax);
                }                
                if (delay_y_ms != 0) {
                    Coordinate ymax = box.size.h;
                    int16_t step = ((TimeMS) time_delta) / delay_y_ms;
                    origin_mod.set_y((step+origin.y+ymax) % ymax);
                } 
                return (RectArea(origin_mod,size_mod));
            };
            virtual void reset(TimeMS delay_x_ms, TimeMS delay_y_ms) { this->delay_x_ms = delay_x_ms;this->delay_y_ms = delay_y_ms;};
            virtual ~PositionModifierLinearMotion() {LOG(" PositionModifierLinearMotion: Destruct");}
    
        private:
            TimeMS delay_x_ms, delay_y_ms;
            RectArea box;

    };
*/
    typedef int8_t SineMotionType;

    /**
     * @brief The origin is being moved around within a box using a sine-wave motion
     */
    class PositionModifierSine : public PositionModifier {

        public:
            PositionModifierSine(LightArray *ar, RectArea box, Point initial, TimeMS delay_x_ms, TimeMS delay_y_ms) 
                : PositionModifier(ar), initial(initial), delay_x_ms(delay_x_ms), delay_y_ms(delay_y_ms), box(box)
                {LOG(" PositionModifierSine: Construct");};
            virtual ~PositionModifierSine() {LOG(" PositionModifierSine: Destruct");}
            virtual RectArea calc_shape(TimeMS time_delta, Point origin, Size size) 
                { 
                    Point origin_mod = origin;
                    if (delay_x_ms != 0) {
                        Dimension width = box.size.w - size.w; 
                        TimeMS calctime = time_delta+delay_x_ms*(initial.x - box.origin.x)/box.size.w;
                        origin_mod.x =sine[(calctime % delay_x_ms)*90/delay_x_ms]*width/200+width/2; 
                    }
                    if (delay_y_ms != 0) {
                        Dimension height = box.size.h - size.h;
                        origin_mod.y=sine[(time_delta % delay_y_ms)*90/delay_y_ms]*height/200+height/2; 
                    }
                    return(RectArea(origin_mod,size));
                } 

        private:
            Point initial;
            TimeMS delay_x_ms, delay_y_ms;
            RectArea box;
    };

    enum SizeLoudnessMode : uint8_t { SIZELD_Static, SIZELD_Beginning, SIZELD_Center, SIZELD_End };

    /**
     * @brief The size of a rectangular area is modified by loudness
     */
    class PositionModifierSizeLoudness : public PositionModifier {

        public:
            PositionModifierSizeLoudness(LightArray* ar, LoudnessBase& lb, LoudnessMode ldmode, SizeLoudnessMode slmodew=SIZELD_Beginning, SizeLoudnessMode slmodeh=SIZELD_Beginning, TimeMS glow=200) 
                : PositionModifier(ar), lb(lb), ldmode(ldmode), slmodew(slmodew), slmodeh(slmodeh), glow(glow) {LOG(" PositionModifierSizeLoudness: Construct");};
        
            virtual ~PositionModifierSizeLoudness() {LOG(" PositionModifierSizeLoudness: Destruct");}

            RectArea calc_shape(TimeMS time_delta, Point origin, Size size) {
                Point origin_mod = origin;
                Size size_mod = size;

                Loudness 
                    env = lb.get_loudness(LD_environment),
                    loud = lb.get_loudness(ldmode);

                if (env > 25) {
                    Loudness ld_ref = sft_ld_ref.value(time_delta,loud);

                    if (slmodew != SIZELD_Static)
                    {            
                        Dimension w = sft_ld_w.normalized(time_delta,loud,ld_ref,size_mod.w);

                        switch (slmodew) {
                            case SIZELD_Center:
                                origin_mod.x=origin.x+((size_mod.w-w) >> 1);
                                break;
                            case SIZELD_End:
                                origin_mod.x=origin.x+size_mod.w-w;
                                break;
                            default:
                                break;
                        };
                        size_mod.w= w;
                    }

                    if (slmodeh != SIZELD_Static) {
                        Dimension h = ldsofth.normalized(time_delta,loud,ld_ref,size_mod.h);

                        switch (slmodeh) {
                            case SIZELD_Center:
                                origin_mod.y=origin.y+((size_mod.h-h) >> 1);                                
                                break;
                            case SIZELD_End:
                                origin_mod.y=origin.y+size_mod.h-h;
                                break;
                            default:
                                break;
                        };
                        size_mod.h=h;
                    }
                    
                } else { size_mod.w=0; size_mod.h=0; }

                return RectArea(origin_mod, size_mod);
            };

        protected:
            LoudnessBase& lb;
            LoudnessMode ldmode;
            SizeLoudnessMode slmodew, slmodeh;
            TimeMS glow;

            Softener<Loudness> sft_ld_ref = Softener<Loudness> (60000);
            Softener<Loudness> sft_ld_w = Softener<Loudness>(glow), ldsofth = Softener<Loudness>(glow);

  };

  /// @brief resizes a shape if dimension falls below a minimum size
  class PositionModifierMinSize : public PositionModifier {
        public:
            PositionModifierMinSize(LightArray* ar, Size size_min) : PositionModifier(ar), size_min(size_min) {LOG(" PositionModifierMinSize: Construct");}
            RectArea calc_shape(TimeMS time_delta, Point origin, Size size) {
                Size size_new = 
                    Size(
                        (size.w > size_min.w) ? size.w : size_min.w,
                        (size.h > size_min.h) ? size.h : size_min.h
                    );
                
                return RectArea(origin,size_new);
            }
            virtual ~PositionModifierMinSize() {LOG(" PositionModifierMinSize: Construct");}

        protected:
            Size size_min;
  };


};
#endif