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