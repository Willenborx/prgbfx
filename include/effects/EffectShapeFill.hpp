/**
 * @file EffectShapeFill.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Effect just showing a \link Shape \endlink
 * @version 0.5
 * @date 2023-11-01
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef EffectShapeFill_hpp
#define EffectShapeFill_hpp

#include <Effect.hpp>
#include <Log.hpp>
#include <Shape.hpp>

namespace prgbfx {

    using namespace prgb;
    
    /// @brief This effect displays a @link Shape @endlink, all further settings are stored in the Shape
    class EffectShapeFill : public Effect {

        Shape& shape;

        public:
            EffectShapeFill(LightArray& ar, Shape& shape) 
                : Effect(ar), shape(shape)  { LOG(" EffectShapeFill: Construct"); }
            virtual ~EffectShapeFill() { LOG (" EffectShapeFill: Destruct");}
            virtual void render_effect(TimeMS time_delta) { 
                if (enabled) shape.drawmod(time_delta); 
            };
            
    };
}
#endif