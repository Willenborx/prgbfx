/**
 * @file Effect.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Implementation of an "effect". The calculateEffect member function will be called to draw the effect depending on the current timestamp
 * @version 0.5
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PRGB_EFFECT_HPP
#define PRGB_EFFECT_HPP


#include <cstdint>
#include <LightArray.hpp>
#include <Shape.hpp>
#include <TimeBase.hpp>
#include <Log.hpp>

namespace prgbfx {

    using namespace prgb;

    template<typename T, T scale=1> T normalize(T current, T max, T target) { 
        T maxv = std::max<T>(current,max); 
        return target*(scale*current/maxv)/scale;        
     }

    // General use values for Direction
    enum Direction {DIR_Left, DIR_Right, DIR_Down, DIR_Up };

    /// @brief Effects derived from this abstract class will paint the effect onto the "canvas" when called. The size of this canvas is defined by the @link prgb::Geometry @endlink of the @link prgb::LightArray @endlink
    class Effect {
        protected:
            bool ended = false;
            bool enabled = true;
            LightArray& ar;
            TimeMS time_start; // \todo review

        public:
            Effect(LightArray& ar) : ar(ar) { LOG("Effect: Construct"); time_start=ar.get_timebase().get_deltatime_ms();};
            virtual ~Effect() { LOG("Effect: Destruct"); }
            
            virtual void render_effect(TimeMS time_delta) = 0;
            virtual bool has_ended() { return false; };
            void reset_start_time(TimeMS time_start) { this->time_start=time_start; };
            inline void disable() { enabled = false; }
            inline void enable() { enabled = true; }
            inline void set_enable(bool state) { enabled = state;}

    };

};
#endif