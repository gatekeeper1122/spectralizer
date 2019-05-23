/**
 * This file is part of spectralizer
 * which is licensed under the GPL v2.0
 * See LICENSE or http://www.gnu.org/licenses
 * github.com/univrsal/spectralizer
 */

#pragma once

#include <graphics/graphics.h>
#include <fftw3.h>

#define AUDIO_SIZE  65536
#define BUFFER_SIZE 1024

namespace source
{
    struct config;
}

namespace audio
{

    extern double smoothing_values[];

/* Base class for audio processing
 * Contains methods for processing read input
 * Derived classes deal with getting the audio input like fifo, or obs audio source
 */
    class audio_processor
    {
    protected:
        /* Complex numbers used for processing by fftw3 */
        fftw_complex* m_fftw_out_l{}, * m_fftw_out_r{};
        fftw_plan m_fftw_plan_l{}, m_fftw_plan_r{};

        /* Array of frequency strengths currently and last cycle */
        int* m_freq_l{}, * m_freq_r{};

        /* Utility arrays used for smoothing etc.
         * m_fall_off: Handles slower falloff
         * m_last_freq / m_last_freqd: Used for smoothing
         * m_low_freq_cut / m_high_freq_cut: Cuts high/low frequencies
         * m_freq_mem: Used for smoothing
         */
        int* m_fall_off{}, * m_last_freqs{}, * m_last_freqsd{}, * m_low_freq_cut{}, * m_high_freq_cut{}, * m_freq_mem{};
        float* m_freq_peak{};
        double* m_freq_weight{}; /* Weights frequencies according to EQ (TODO: used?) */
        double m_eq_dist{}; /* Distributes equalizer values across all bars */

        /* Real part for fftw
         * Copied over from m_audio_out
         * TODO: redundant? data type potentially int?
         */
        double* m_fftw_in_l{}, * m_fftw_in_r{};

        float m_sleep_counter{};

        int32_t m_buf_size{};
        uint8_t m_channels{};
    public:
        int32_t m_samples{}; /* 2 * (m_buf_size / 2 + 1) */
        int16_t m_audio_out_r[AUDIO_SIZE]{}; /* Contains audio data read from audio source */
        int16_t m_audio_out_l[AUDIO_SIZE]{};

        audio_processor(source::config* cfg);

        virtual ~audio_processor();

        /* Setup/Cleanup */
        virtual void clean_up();

        virtual void update(source::config* cfg);

        /* Audio processing methods */
        virtual void tick(float seconds, source::config* cfg);

        void separate_freq_bands(source::config* cfg, uint16_t detail, bool left_channel);

        uint8_t get_channels();

        int32_t get_buffer_size();
    };

}
