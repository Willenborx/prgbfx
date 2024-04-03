/**
 * @file    Scene.hpp
 * @author  Holger Willenborg (holger@willenb.org)
 * @brief   This class implements the "Scene" which controls the Effect chain. 
 *          Effects are added to the Scene and the Scene is calculating the frames from the 
 *          list of Effects. It also checks if effects are expired and removes them from the scene.
 * @version 0.6
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef PRGB_SCENE_HPP
#define PRGB_SCENE_HPP

#include <LightArray.hpp>
#include <Effect.hpp>
#include <TimeBase.hpp>
#include <SoundObserver.hpp>


#include <list>

/**
 * @brief The prgbfx namespace contains classes to implement an effect generator. It relies on classes 
 *        from the prgb namespace and does not contain any hardware-dependent code. That means it's possible
 *        to use this part of the code on different hardware without any porting effort.
 *        The prgb classes which are providing the hardware abstraction layer may be implemented for all
 *        kinds of hardware
 */
namespace prgbfx {
    using namespace prgb;

    /// @brief The Scene class runs the actual calculation of the effects and commits the buffer (which switches the display to the newly calculated buffer). It uses
    ///        two effect chains which may be switched
    class Scene {
            protected:
                TimeBase& tb;
                LightArray* ar;
                EffectChain *fx_chain = nullptr;

                LoudnessBase& lb;
                SoundObserver observe = SoundObserver(lb,tb);

                uint64_t frames = 0;

                bool bStop = false;

            public:
                Scene(LightArray* ar, TimeBase& tb, LoudnessBase& lb): tb(tb),ar(ar),lb(lb) { LOG("Scene: Construct");};
                ~Scene() { LOG("Scene: Destruct");};

                /// @brief runs the scene and calculates the frames. Calls PreFrame, PreEffect, PostEffect, PreCommit, PostFrame which may be
                ///        implemented in derived classes to do specific actions during the run. runScene needs to be run in an infinite loop
                ///        that checks is_stopped(). runScene uses the active EffectChain
                void runScene() {
                    ar->fill_all(RGBA(0,0,0,255));
                    TimeMS delta = tb.get_deltatime_ms();

                    pre_frame(delta);
                    fx_chain->pre_frame(delta);

                    std::vector<Effect *> *fx_list = fx_chain->get_effects_list();
                    for (std::vector<Effect *>::iterator it = fx_list->begin(); it != fx_list->end(); ) {
                        Effect *e = *it;
                        pre_effect(delta,e);
                        e->render_effect(delta);
                        if (e->has_ended()) { it = fx_list->erase(it); } else { ++it; }
                        post_effect(delta,e);
                    }

                    frames++;
                    pre_commit(delta);
                    ar->commit_buffer();

                    // collect sound data into the observer
                    observe.collect_sound_data(delta);

                    post_frame(delta);
                    fx_chain->post_frame(delta);
                };

                LightArray* get_array() { return this->ar; }
                TimeBase& get_timebase() { return tb; }

                /// @brief  get_frame_count for statistical reasons
                /// @return 
                uint64_t get_frame_count() { return frames; }

                /// @brief  Get Effect count for statistical reasons
                /// @return 
                size_t  get_effect_count() { return ((size_t) fx_chain->get_effects_list()->size());}

                /// @brief  pre_frame will be called before a frame is calulcated
                /// @param time_delta timestampe 
                inline virtual void pre_frame(TimeMS time_delta) {}

                /// @brief pre_commit will be called right before the entire frame is committed to the output
                /// @param time_delta 
                inline virtual void pre_commit(TimeMS time_delta) {};

                /// @brief  post_frame will be called after a frame has been calculated and commited
                /// @param time_delta 
                inline virtual void post_frame(TimeMS time_delta) {}

                /// @brief  pre_effect will be called before an effect will be calculated
                /// @param time_delta 
                /// @param e 
                inline virtual void pre_effect(TimeMS time_delta, Effect *e) {}

                /// @brief  post_effect will be called after an effect has been calculated
                /// @param time_delta 
                /// @param e 
                inline virtual void post_effect(TimeMS time_delta, Effect *e) {}

                /// @brief stops the scene
                inline void stop() { bStop = true; } 

                /// @brief return true if scene is top be stopped
                inline bool is_stopped() { return bStop; }

    };
};
#endif