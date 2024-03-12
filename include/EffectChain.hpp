/**
 * @file EffectChain.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief THe EffectChain manages a list of effects. 
 * @version 0.6
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
  * 
 */

#ifndef PRGB_EFFECTCHAIN_HPP
#define PRGB_EFFECTCHAIN_HPP

#include <Effect.hpp>
#include <SoundObserver.hpp>
#include <list>
#include <Log.hpp>

namespace prgbfx {
    
    using namespace prgb;

    /**
     * @brief An EffectChain combines effects. The effects are stored in a vector and the effect chain manages this list of effects. 
     *        the @link pre_frame() @endlink and @link post_frame() @endlink member functions are called before any/after all effects
     *        have been renederes
     */
    class EffectChain {

        public:
            EffectChain(LightArray* ar, LoudnessBase& lb, SoundObserver& ob) : ar(ar), lb(lb),ob(ob) { 
                LOG("EffectChain: Constructor");
            };
            
            virtual ~EffectChain() { 
                LOG("EffectChain: Destruct");
            }

            virtual void add(Effect *effect) { 
                LOG("EffectChain: Add effect"); 
                effects.get()->push_back(effect); 
            }

            std::vector<Effect *> *get_effects_list() { return effects.get(); }

            virtual inline void pre_frame(TimeMS delta_time) {}
            virtual inline void post_frame(TimeMS delta_time) {}

        protected:
            std::unique_ptr<std::vector<Effect *>> effects = std::make_unique<std::vector<Effect *>>();
            LightArray* ar;
            LoudnessBase& lb;
            SoundObserver& ob;
    };
}

#endif