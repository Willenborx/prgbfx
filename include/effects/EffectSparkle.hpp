/**
 * @file EffectSparkle.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief Add sparkles to the output.
 * @version 0.2
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef EffectSparkle_hpp
#define EffectSparkle_hpp

#include <Effect.hpp>
#include <effects/EffectArrayAbstract.hpp>
#include <LoudnessBase.hpp>
#include <TimeBase.hpp>
#include <vector>
#include <Log.hpp>

namespace prgbfx {

    using namespace prgb;

    /**
     * @brief This is used in conjunction with @link Effect @endlink and contains information about
     *        a single pixel that appears and then fades away over a delay time
     */
    struct Spark {
        Point origin;
        TimeMS time_start;
        TimeMS delay;
        ColorValue color;
    };

    /**
     * @brief Implements a sparkle effect that adds Sparks over time with slightly randomized "burn" times
     *        The sparks disapper after a while (getting transparent)
     * 
     */
    class EffectSparkle : public EffectArrayAbstract<Spark> {

        LoudnessBase& lb;
        RectArea box;
        uint16_t density;

        EffectColor* color;
        ColorModifiers colmods;

        Softener<Loudness> peak = Softener<Loudness>(1000);

        TimeMS min_spark_duration, max_spark_duration, avg_spark_duration ;
        TimeMS delay_sparkles = 0, delay_sparkles1000 = 0;
        TimeMS last_sparkle = 0;

        bool hibernate = false;

        public:


            EffectSparkle(LightArray* ar, LoudnessBase& lb, const RectArea& box, uint16_t density, EffectColor* color, ColorModifiers colmods, TimeMS min_spark_duration=20, TimeMS max_spark_duration=180) 
                    : EffectArrayAbstract(ar), lb(lb), box(box), density(density), color(color), colmods(colmods),min_spark_duration(min_spark_duration), max_spark_duration(max_spark_duration) { 
                avg_spark_duration = min_spark_duration + (max_spark_duration-min_spark_duration)/2;
                //delay_sparkles1000 = (int32_t)(((int64_t)1000*1000*avg_spark_duration) / (int64_t) (box.size.h*box.size.w*density));
                //delay_sparkles = delay_sparkles1000 / 1000;
                set_density(density);
                LOG("  EffectSparkle: Construct");
            }

            virtual ~EffectSparkle() {LOG("  EffectSparkle: Destruct");}

            void set_density(uint16_t density) { 
                delay_sparkles1000 = (int32_t)(((int64_t)1000*1000*avg_spark_duration) / (int64_t) (box.size.h*box.size.w*density));
                delay_sparkles = delay_sparkles1000 / 1000;
            }

            virtual void render_effect(TimeMS time_delta) {

                if (hibernate && enabled) last_sparkle = 0; // reset timer after reactivation

                if (last_sparkle == 0) last_sparkle = time_delta; // initial call -> set some time

                // Add sparkles after a certain time (if not disabled)
                if ((time_delta > last_sparkle+delay_sparkles) && enabled) {

                    int32_t num_sparks = (int32_t)(1000*(int32_t)(time_delta - last_sparkle) / (int32_t) delay_sparkles1000);
                    if (num_sparks > 0) LOG("  EffectSparkle: Adding effects -> "+std::to_string(num_sparks));

//                    for (int32_t i = 0; i < std::min(num_sparks,(int32_t) 100) ; i++) { // #desperate attempt to fix a crash
                    for (int32_t i = 0; i < num_sparks ; i++) { 
                        ColorValue color_new = color->get_color(time_delta);

                        for (auto cmod : colmods) {
                                color_new = cmod->modify(color_new,time_delta);
                        }

                        add_item     (Spark(
                                        {   
                                            Point(rand()%box.size.w,rand()%box.size.h),
                                            time_delta,
                                            min_spark_duration+rand()%(max_spark_duration-min_spark_duration),
                                            color_new
                                        }
                                    ));
                    }
                    last_sparkle = time_delta;
                }

                hibernate = !enabled;


                // for (auto cmod : colmods) {
                //         color_new = cmod->modify(color_new,time_delta);
                // }
                LOG("  EffectSparkle: Start output: Size -> "+std::to_string(items.size()));
                
                for_each([this, time_delta](Spark& spark){
                    if (time_delta-spark.time_start >= spark.delay) {
                        // LOG("  EffectSparkle: Erase Spark");
                        return false;
                    } else {
                        // #desperate attempt to sanitize this..
                        int32_t opacity = (spark.time_start > time_delta) ? 0 : 100-(100*(time_delta - spark.time_start)) / spark.delay;

                        opacity = (opacity > 100) ? 100 : opacity; // : (opacity > 1000) ? 0 : opacity; /// \todo evaluate

                        ar->set_pixel(
                                Point(spark.origin.x+box.origin.x,spark.origin.y+box.origin.y),
                                spark.color,
                                CMODE_Transparent,
                                (int8_t) opacity);
                        return true;
                    }
                });
                
            }

    };
};
#endif