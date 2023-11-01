/**
 * @file Limiter.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Class template to limit the input value to a defined maximum 
 * @version 0.1
 * @date 2023-05-06
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef PRGB_LIMITER_HPP
#define PRGB_LIMITER_HPP

#include <stdint.h>

namespace prgbfx {

    /**
     * @brief Limits a value to a top value by using a function that converges to one maximum value
     *        Any value passed to the limit() function cannot exceed a predefined maximum
     * 
     * @tparam T the value type
     */
    template <class T>
    class Limiter {
        public:
        Limiter() { reset(1000,200); } // default setting
        Limiter(T val_max, T val_x80) : val_max(val_max),  val_x80(val_x80) {};
        T limit(T rawvalue) {
            T ltdval = val_max*(1000-(val_x80/4*1000)/(val_x80/4+rawvalue))/1000;
            return ltdval;
        }  
        void reset(T val_max, T val_x80) {
            this->val_max = val_max;
            this->val_x80 = val_x80;
        }

    protected:
        T val_max;
        T val_x80;
    };
}

#endif
