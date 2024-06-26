/**
 * @file SoundObserver.hpp
 * @author Holger Willenborg (holger@willenb.org)
 * @brief  Soundobserver continuously collects sound data to assess certain parameters of the music. It sets flags which may be used to 
 *         select the next effect or to spawn new effects
 * @version 0.6
 * @date 2024-03-12
 * 
 * @copyright Copyright (c) 2024
 */

#ifndef _SOUNDOBSERVER_HPP
#define _SOUNDOBSERVER_HPP

#include <LoudnessBase.hpp>
#include <TimeBase.hpp>

namespace prgbfx {

    using namespace prgb;

    typedef uint16_t ObserverFlags;

    /**
     * @brief This class will be called when displaying a frame. It collects and assesses the sound data provided
     *        by the @link LoudnessBase @endlink class (Loudness / Frequency Band information), sets flags and provides
     *        data to help spawn the next effect or to select proper followup effects
     */
    class SoundObserver {

        ObserverFlags flags=0;

        TimeMS nobass_timestamp = 0;
        Softener<Loudness> ld_soft = Softener<Loudness>(2000);
        Softener<Loudness> ld_norm = Softener<Loudness>(250);
        Softener<int> ld_0_255soft = Softener<int>(150);

        double ld_linreq_slope = 0.0;
        double ld_linreq_offset = 0.0;
        int ld0_255 = 0;
        int ld_delta = 0;

        public:
            const static TimeMS time_nobass_threshold = 4000; // time span to detect a fade out / build up
            const static TimeMS time_linreg_sample = 1500;    // length of the sample to be used for build up
            const static int16_t ld_linreg_sample_count = 10; // number of data points for linear regression
            const static int32_t ld_linreg_sample_length = time_linreg_sample/ld_linreg_sample_count;

            enum ObserverFlag:uint8_t { SO_Silence=0, SO_LoudnessPeak=1, SO_NoBass=2, SO_Buildup=3, SO_FadeOut=4, SO_DynamicPeak=5, SO_PeakHigh=6, SO_PeakLow=7 };

            // Array to calculate linear regression of loudness development
            LoudnessDB ld_linreg[ld_linreg_sample_count];
            int32_t ld_linreg_idx = 0;
            int32_t ld_linreg_ct = 0;
            TimeMS ld_linreg_start = 0;

            SoundObserver(LoudnessBase& lb, TimeBase& tb) : lb(lb), tb(tb) {  }

            /// @brief Collects sound data to assess the current soundscape. Collected data will be used
            ///        to select the next effect 
            ///         \todo implement this
            /// @param time_delta 
            void collect_sound_data(TimeMS time_delta) {

                    if (nobass_timestamp == 0) nobass_timestamp = tb.get_deltatime_ms();

                    Loudness ld_env = lb.get_loudness(LD_environment);
                    LoudnessDB ld_env_db =  lb.get_db_value(ld_env);
                    Loudness ld_real = lb.get_loudness(LD_Realtime);


                    // dynamic peak is when the current value exceeds the softenend old peak
                    if (ld_soft.value(time_delta,ld_real) == ld_soft.get_value_peak()) {
                        set_flag(SO_DynamicPeak);
                    } else {
                        clear_flag(SO_DynamicPeak);
                    }


                    Loudness ld_pre = ld_norm.get_value();
                    Loudness ld_now = ld_norm.value(time_delta,ld_real);

                    // try to see dynamics
                    double ld_delta_prenow = lb.get_db_value(ld_now) - lb.get_db_value(ld_pre);



                    // try to normalize loudness to a value between 0-100
                    // expect dynamic range of +/- 10 dB
                    double ld_delta_db = (lb.get_db_value(ld_now) - lb.get_db_value(ld_env)) + 10; // current softened real value against env value
                    ld_delta_db = 13 * ((ld_delta_db < 0) ? 0 : (ld_delta_db > 20) ? 20 : ld_delta_db); 

                 ld0_255 = ld_0_255soft.value(time_delta, static_cast<Loudness>(ld_delta_db));
                 ld0_255 = ld0_255 * ld0_255 / 255;

                    // Quite ld_env_dbironment
                    if (lb.is_silent()) {
                        set_flag(SO_Silence);
                        clear_flag(SO_PeakHigh);
                        clear_flag(SO_PeakLow);
                    } else if (lb.is_not_silent()) {
                        clear_flag(SO_Silence);
                        set_flag_state(SO_PeakHigh,(ld_delta_prenow > 9.0));
                        set_flag_state(SO_PeakLow,(ld_delta_prenow < -9.0));
                        ld_delta = static_cast<int>(ld_delta_prenow*10.0);

                    }
                    
                    // \todo hysteresis: Silence -> <60dB, no Silence -> >63dB                    
                    if (lb.get_loudness_db(LD_Realtime) >= (ld_env_db + 3.0)) {
                        if (!is_flag_set(SO_Silence)) set_flag(SO_LoudnessPeak);
                    } else {
                        clear_flag(SO_LoudnessPeak);
                        clear_flag(SO_NoBass);
                    }

                    //  detect missing bass tones
                    if (lb.get_loudness(LD_Band_Bass) < ld_env_db) {
                        if ((time_delta - nobass_timestamp) > time_nobass_threshold) {
                            flags |= (1 << SO_NoBass);
                        }
                    } else {
                        nobass_timestamp = time_delta;
                    }

                    // build up? fade out? Check if the average constantly going up/down.

                    if (ld_linreg_start == 0) ld_linreg_start = time_delta; // Initialize start time stamp

                    // calculate index in sample array
                    //int32_t idx = (((time_delta-ld_linreg_start) % (time_linreg_sample)) / ld_linreg_sample_length);
                    int64_t idx = ((time_delta-ld_linreg_start)/ld_linreg_sample_length) % ld_linreg_sample_count;

                    // time for a new sample?
                    if (idx != ld_linreg_idx) {
                        ld_linreg[idx] = ld_env_db;
                        ld_linreg_ct++;

                        // don't start before the entire time span is considered
                        if (ld_linreg_ct >= ld_linreg_sample_count) {
                            
                            // Do linear regression
                            double dsumX=0, dsumY=0,dsumX2=0, dsumXY=0;
                            for (int16_t i=0; i< ld_linreg_sample_count; i++) {
                                // real index: start at current point in time
                                int32_t ri = (ld_linreg_idx+i) % ld_linreg_sample_count;
                                // x value in ms
                                double x = static_cast<double>(i*ld_linreg_sample_length);
                                double y = static_cast<double>(ld_linreg[ri]);
                                
                                // sum up
                                dsumX += x;
                                dsumX2 += x*x;
                                dsumY += y;
                                dsumXY += x*y;
                            }

                            double dcount = static_cast<double>(ld_linreg_sample_count);
                            
                            ld_linreq_slope = (dcount*dsumXY - dsumX*dsumY) / (dcount*dsumX2-dsumX*dsumX);
                            ld_linreq_offset = (dsumY - ld_linreq_slope*dsumX) / dcount;
                        }
                        ld_linreg_idx = idx;

                    }

                    if (get_ld_linreq_slope() > 1.0) set_flag(SO_Buildup); else clear_flag(SO_Buildup);
                    if (get_ld_linreq_slope() < -1.0) set_flag(SO_FadeOut); else clear_flag(SO_FadeOut);


            }

            inline double get_ld_linreq_slope() { return 1000*ld_linreq_slope;}
            inline double get_ld_linreq_offset() { return ld_linreq_offset;}

            ObserverFlags get_flags() { return flags; }

            inline bool is_flag_set(ObserverFlags flag) { return (flags & (1 << flag));}
            
            inline void set_flag(ObserverFlag flag) {
                flags |= (1 << flag);
            }

            inline void clear_flag(ObserverFlag flag) {
                flags &= ~(1 << flag);
            }

            inline void set_flag_state(ObserverFlag flag, bool set) {
                if (set) set_flag(flag); else clear_flag(flag);
            }

            inline int16_t get_ld_0_100() {
                return  std::min(100,(ld0_255*100)/255);
            }

            inline uint8_t get_ld_0_255() {
                return ld0_255;
            }

            inline int get_delta() {
                return ld_delta;
            }

        protected:
            Loudness loudness_last_avg = 0;

            TimeMS time_last_avg_low = 0;
            TimeMS time_last_avg_high = 0;

            Loudness loudness_last_avg_low = 0;
            Loudness loudness_last_avg_high = 0;

            LoudnessBase& lb;
            TimeBase& tb;

    };

}

#endif