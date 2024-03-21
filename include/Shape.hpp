/**
 * @file Shape.hpp
 * @author Holger Willenborg
 * @brief A Shape is some object that can be painted into a rectangular area. The effect control may modify the position and size
 *        of a shape at any time so any implementation needs to be able to draw the shape into any reactangle
 * @version 0.6
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
  * 
 */
#ifndef PRGB_SHAPE_HPP
#define PRGB_SHAPE_HPP


#include <stdint.h>
#include <LightArray.hpp>
#include <TimeBase.hpp>
#include <Sine.hpp>
#include <ColorModifier.hpp>
#include <EffectColor.hpp>
#include <PositionModifier.hpp>

namespace prgbfx {

    using namespace prgb;
    /**
     * @brief The Shape class is the abstract base class for objects that can be drawn into a rectangular area. Shapes do not need to overwrite the complete area, 
     * but the shape position and dimension and size are always described by a rectangle. This makes possible an implementation of a general @link PositionModifier @endlink
     * which changes the shape based on parameters like time or loudness. Any shape must be able to present itself in the rectangluar area assigned to it.
     * By default a shape can take the following parameters:
     * - Origin and Size (or Box) describe the rectangular area
     * - The PositionModifier array is a vector of position modifiers that are processed before each presentation. These modifiers can relocate and resize the rectangular area of the shape
     * - The EffectColor is the base color of the shape, @link EffectColor @endlink can also provide changing colors
     * - The ColorModifiers  can modifiy this base color by applying changes based on parameters like loudness or time (or others)
     */
    class Shape {
        public:
            //Shape(LightArray* ar, Point origin, Size size, vector<PositionModifier *>posmods, EffectColor* color, ColorMode mode=CMODE_Set);
            Shape(LightArray* ar, RectArea box, PositionModifiers posmods, EffectColor* color, ColorMode mode=CMODE_Set, ColorModifiers colmods={}, int8_t opacity=100)
                     : ar(ar), box(box), posmods(posmods), colmods(colmods), color(color), mode_color(mode), opacity(opacity) {
                LOG("Shape: Construct");
            }

            virtual ~Shape() {LOG("Shape: Destruct");}
            /// @brief  Must be implemented by all derived classes and draws the shape at the origin and size passed (those might have been modified by PositionModifiers) and the timestamp
            /// @param time_delta 
            virtual void draw(Point origin, Size size, TimeMS time_delta) = 0;
            /// @brief this applies the PositionModifiers and calls draw.
            /// @param time_delta 
            void drawmod(TimeMS time_delta){
                RectArea modarea = box;
                
                for (auto pmod : posmods) {
                    modarea = pmod->calc_shape(time_delta,modarea.origin,modarea.size);
                }

                draw(modarea.origin, modarea.size,time_delta);
            }

            /// @brief  changes the origin
            /// @param origin 
            void set_origin(Point origin) { this->box.origin = origin; }

            /// @brief  Set the opacity
            /// @param opacity 100 means the object has no transparence
            void set_opacity(int8_t opacity) { this->opacity = opacity; }

            inline Point get_origin() { return box.origin; };
            inline Size get_size() { return box.size; };

        protected:
            LightArray* ar;
            RectArea box;
            PositionModifiers posmods = PositionModifiers ();
            ColorModifiers  colmods = ColorModifiers  ();
            EffectColor* color;
            ColorMode mode_color;
            uint8_t opacity = 100;

            ColorValue get_color(TimeMS time_delta, ColorValue color = 0) {
                ColorValue color_new = color;

                for (auto cmod : colmods) {
                        color_new = cmod->modify(color_new,time_delta);
                }

                return color_new;                
            }

    };


    //! Initializer struct for a Rect object with sensible default values.
    struct RectInit {
        RectArea box;
        PositionModifiers       posmods;
        EffectColor*            color;
        ColorMode               colormode   {CMODE_Set};
        ColorModifiers          colmods;
        int8_t                  opacity     {100};
    };

    /// @brief A filled rectangle shape that can be assigned positional or color modfiers
    class Rect : public Shape {

        public:
            Rect(LightArray* ar, const RectInit& ri) 
                : Shape(ar,ri.box,ri.posmods,ri.color, ri.colormode, ri.colmods, ri.opacity) {LOG(" Rect: Construct");}

            Rect(LightArray* ar,RectArea box, PositionModifiers posmods, EffectColor* color, ColorMode mode=CMODE_Set, ColorModifiers colmods ={}, int8_t opacity=100)
                : Shape(ar,box,posmods,color, mode, colmods, opacity) {LOG(" Rect: Construct"); }
            
            virtual ~Rect() {LOG(" Rect: Destruct");}

            virtual void draw(Point origin, Size size, TimeMS time_delta){
                ColorValue color_current = color->get_color(time_delta);
                ColorValue color_new = get_color(time_delta, color_current);
                
                for (uint16_t cx = 0; cx < size.w; cx++) {
                    for (uint16_t cy = 0; cy < size.h; cy++) {
                        ar->set_pixel(Point(cx+origin.x,cy+origin.y),color_new,mode_color,opacity);
                    }
                }   
            }
    };



    //! Initializer struct for a Rect object with sensible default values.
    struct FrameInit {
        RectArea            box;
        PositionModifiers   posmods;
        EffectColor*        color;
        ColorMode           mode        {CMODE_Set};
        ColorModifiers      colmods;
        Dimension           width_frame {1};
        int8_t              opacity     {100};
    };

    /**
     * @brief A frame shape which can be just an unfilled rectangle but can also use a different thickness
     */
    class Frame : public Shape {

        Dimension width_frame;

        public:
            Frame(LightArray* ar, const FrameInit fi)
                : Shape(ar,fi.box,fi.posmods,fi.color,fi.mode,fi.colmods,fi.opacity), width_frame(fi.width_frame) {LOG(" Frame: Create");};

            Frame(LightArray* ar, RectArea box, PositionModifiers posmods, EffectColor* color, ColorMode mode,ColorModifiers colmods,Dimension width_frame=1, int8_t opacity=100) 
                : Shape(ar,box,posmods,color,mode,colmods,opacity), width_frame(width_frame) {LOG(" Frame: Create");};

            virtual void draw(Point origin, Size size, TimeMS time_delta) {
                ColorValue color_current = color->get_color(time_delta);
                ColorValue color_new = get_color(time_delta, color_current);

                if ((size.h < 2) || (size.w < 2) ) return;

                for (int j = 0; j < std::min(width_frame,(Dimension) (size.h/2)); j++) {
                    for (int i=0;i < size.w; i++) {
                        ar->set_pixel(Point(origin.x+i,origin.y+j),color_new,mode_color,opacity);
                        ar->set_pixel(Point(origin.x+i,origin.y+size.h-j-1),color_new,mode_color,opacity);
                    }
                }
                for (int i=0; i< std::min(width_frame,(Dimension)(size.w/2)); i++) {
                    for (int j=width_frame; j<size.h-width_frame; j++) {
                        ar->set_pixel(Point(origin.x+i,origin.y+j),color_new,mode_color,opacity);
                        ar->set_pixel(Point(origin.x+size.w-i-1,origin.y+j),color_new,mode_color,opacity);
                    }
                }
            }
            virtual ~Frame() {LOG(" Frame: Destruct");}

    };

    struct CircleInit {
        RectArea            box;
        PositionModifiers   posmods;
        EffectColor*        color;
        ColorMode           mode        {CMODE_Set};
        ColorModifiers      colmods;
        int8_t              opacity     {100};
    };

    class Circle : public Shape {

            Point center; 

            public:
                Circle(LightArray* ar, const CircleInit ci) 
                    : Shape(ar,ci.box,ci.posmods,ci.color,ci.mode,ci.colmods,ci.opacity),center(ci.box.size.w/2-1,ci.box.size.h/2-1) { 
                    };
                
                virtual void draw(Point origin, Size size, TimeMS time_delta) {

                    if ((center.x == 0) || (center.y == 0)) return;

                    ColorValue color_current = color->get_color(time_delta);
                    ColorValue color_new = get_color(time_delta, color_current);

                    // for (int i=0; i<= size.w; i++) {
                    //     for(int j=0; j <= size.h; j++) {
                    //         Dimension x = abs(center.x - i*10);
                    //         Dimension y = abs(center.y - j*10);
                    //         if (10*x*x/(center.x*center.x) + 10*y*y/(center.y*center.y) <= 10) {
                    //             Point p1 = Point(origin.x+i,origin.y+j);
                    //             ar->set_pixel(Point(p1.x,p1.y),color_new,mode_color,opacity);
                    //         }
                    //     }
                    // }

                    int32_t height = (int32_t)size.h/2; //+(size.h&1);
                    int32_t width = (int32_t)size.w/2; //+(size.w&1);

                    int32_t hh = height*height;
                    int32_t ww = width*width;
                    
                    int32_t h2w2 = height*height*width*width;

                    for(int32_t y=-height; y<=height; y++) {
                        for(int32_t x=-width; x<=width; x++) {
                            if(x*x*hh+y*y*ww <= h2w2)
                                ar->set_pixel(Point(origin.x+width+x,origin.y+height+y),color_new,mode_color,opacity);
                        }
                    }
                }
    };

};
#endif