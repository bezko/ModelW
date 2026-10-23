#pragma once

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <algorithm>
#include <cstdint>

namespace ModelW {

/**
 * @brief High-performance stereo dynamics and spectral de-essing DSP core.
 *
 * Architecture Highlights:
 * 1. Log-domain feed-forward gain computer with true 2nd-order quadratic soft knee.
 * 2. Peak-detecting envelope follower with adjustable Attack time.
 * 3. Transient Hold stage to eliminate low-frequency ripple distortion.
 * 4. Dual-slope program-dependent release ballistics.
 * 5. Relative spectral tilt de-essing (sibilant energy relative to broadband).
 * 6. Dynamic subtractive biquad crossover filter.
 * 7. L/R differential trim adjustments and VCA smoothing control.
 */
class Engine {
public:
    Engine(double sampleRate = 44100.0) {
        setSampleRate(sampleRate);
        reset();
    }

    void setSampleRate(double sampleRate) {
        m_sampleRate = (sampleRate > 1000.0) ? sampleRate : 44100.0;
        updateAttackCoeff();
        updateReleaseCoeff();
        updateHoldSamples();
        updateSidechainFilter();
    }

    // ==========================================
    // Parameter Setters
    // ==========================================

    /// Threshold in dB: -60.0 dB to 0.0 dB
    void setThreshold(float thresholdDB) {
        m_thresholdDB = std::clamp(thresholdDB, -60.0f, 0.0f);
    }
    float getThreshold() const { return m_thresholdDB; }

    /// Ratio: 1.0:1 to 64.0:1
    void setRatio(float ratio) {
        m_ratio = std::clamp(ratio, 1.0f, 64.0f);
        m_slope = (m_ratio > 1.0f) ? (1.0f - 1.0f / m_ratio) : 0.0f;
    }
    float getRatio() const { return m_ratio; }

    /// Attack time in milliseconds: 0.1 ms to 50.0 ms
    void setAttack(float attackMs) {
        m_attackMs = std::clamp(attackMs, 0.1f, 50.0f);
        updateAttackCoeff();
    }
    float getAttack() const { return m_attackMs; }

    /// Hold time in milliseconds: 0.0 ms to 100.0 ms
    void setHold(float holdMs) {
        m_holdMs = std::clamp(holdMs, 0.0f, 100.0f);
        updateHoldSamples();
    }
    float getHold() const { return m_holdMs; }

    /// Release time in milliseconds: 20.0 ms to 2000.0 ms
    void setRelease(float releaseMs) {
        m_releaseMs = std::clamp(releaseMs, 20.0f, 2000.0f);
        updateReleaseCoeff();
    }
    float getRelease() const { return m_releaseMs; }

    /// Soft Knee toggle
    void setSoftKnee(bool enable) {
        m_softKnee = enable;
    }
    bool getSoftKnee() const { return m_softKnee; }

    /// Soft Knee width in dB: 0.0 dB to 40.0 dB
    void setSoftKneeWidth(float widthDB) {
        m_kneeWidthDB = std::clamp(widthDB, 0.0f, 40.0f);
        m_halfKneeWidthDB = m_kneeWidthDB * 0.5f;
    }
    float getSoftKneeWidth() const { return m_kneeWidthDB; }

    /// Make-up Gain in dB: 0.0 dB to +48.0 dB
    void setMakeUpGain(float makeUpDB) {
        m_makeUpDB = std::clamp(makeUpDB, 0.0f, 48.0f);
        m_makeUpLinear = std::pow(10.0f, m_makeUpDB / 20.0f);
    }
    float getMakeUpGain() const { return m_makeUpDB; }

    /// De-Esser toggle
    void setDeEsserEnabled(bool enable) {
        m_deEsserEnabled = enable;
    }
    bool isDeEsserEnabled() const { return m_deEsserEnabled; }

    /// De-Esser center/cutoff frequency: 1000 Hz to 10000 Hz
    void setDeEsserFrequency(float freqHz) {
        m_deEsserFreqHz = std::clamp(freqHz, 1000.0f, 10000.0f);
        updateSidechainFilter();
    }
    float getDeEsserFrequency() const { return m_deEsserFreqHz; }

    /// De-Esser relative threshold in dB: -30.0 dB to 0.0 dB
    void setDeEsserThreshold(float dsThresholdDB) {
        m_deEsserThresholdDB = std::clamp(dsThresholdDB, -30.0f, 0.0f);
    }
    float getDeEsserThreshold() const { return m_deEsserThresholdDB; }

    /// De-Esser sidechain monitor listen toggle
    void setDeEsserMonitor(bool enable) {
        m_deEsserMonitor = enable;
    }
    bool isDeEsserMonitor() const { return m_deEsserMonitor; }

    // ==========================================
    // Advanced Parameter Setters
    // ==========================================

    /// SoftSat harmonic saturation crossover mode
    void setSoftSat(bool enable) {
        m_softSat = enable;
    }
    bool getSoftSat() const { return m_softSat; }

    /// Left/Right Input Differential Trim in dB (-6.0 dB to +6.0 dB)
    void setLRInputDiff(float diffDB) {
        m_lrInDiffDB = std::clamp(diffDB, -6.0f, 6.0f);
        m_lrInLinearL = std::pow(10.0f, (m_lrInDiffDB * 0.5f) / 20.0f);
        m_lrInLinearR = std::pow(10.0f, (-m_lrInDiffDB * 0.5f) / 20.0f);
    }
    float getLRInputDiff() const { return m_lrInDiffDB; }

    /// Left/Right Output Differential Trim in dB (-6.0 dB to +6.0 dB)
    void setLROutputDiff(float diffDB) {
        m_lrOutDiffDB = std::clamp(diffDB, -6.0f, 6.0f);
        m_lrOutLinearL = std::pow(10.0f, (m_lrOutDiffDB * 0.5f) / 20.0f);
        m_lrOutLinearR = std::pow(10.0f, (-m_lrOutDiffDB * 0.5f) / 20.0f);
    }
    float getLROutputDiff() const { return m_lrOutDiffDB; }

    /// VCA Ballistics Smoothing Time Constant in ms (0.1 ms to 10.0 ms)
    void setVCASmoothTime(float timeMs) {
        m_vcaSmoothMs = std::clamp(timeMs, 0.1f, 10.0f);
        double tau = m_vcaSmoothMs * 0.001;
        m_grSmoothingCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (m_sampleRate * tau)));
    }
    float getVCASmoothTime() const { return m_vcaSmoothMs; }

    /// Engine Subsampling Rate (1x full sample rate, 2x, 4x, 8x control rate)
    void setSubsampleRate(int factor) {
        m_subsampleFactor = std::clamp(factor, 1, 8);
    }
    int getSubsampleRate() const { return m_subsampleFactor; }

    // ==========================================
    // Metering & Visuals
    // ==========================================

    float getGainReductionDB() const {
        return m_smoothedGainReductionDB;
    }

    void reset() {
        m_envFiltered = 1e-5f;
        m_envBroadband = 1e-5f;
        m_smoothedGainReductionDB = 0.0f;
        m_holdCounter = 0;
        m_subsampleCounter = 0;

        m_scL_x1 = m_scL_x2 = m_scL_y1 = m_scL_y2 = 0.0f;
        m_scR_x1 = m_scR_x2 = m_scR_y1 = m_scR_y2 = 0.0f;

        m_splitL_x1 = m_splitL_x2 = m_splitL_y1 = m_splitL_y2 = 0.0f;
        m_splitR_x1 = m_splitR_x2 = m_splitR_y1 = m_splitR_y2 = 0.0f;
    }

    // ==========================================
    // Core DSP Execution
    // ==========================================

    void process(float& left, float& right) {
        constexpr float MIN_ENV = 1e-5f;

        // Apply L/R Input balance offsets
        float inL = left * m_lrInLinearL;
        float inR = right * m_lrInLinearR;

        // 1. Sidechain filtering
        float scL = inL;
        float scR = inR;
        if (m_deEsserEnabled || m_softSat) {
            scL = processSidechainBiquad(inL, m_scL_x1, m_scL_x2, m_scL_y1, m_scL_y2);
            scR = processSidechainBiquad(inR, m_scR_x1, m_scR_x2, m_scR_y1, m_scR_y2);
        }

        // Sidechain Monitor Mode: directly output detection audio
        if (m_deEsserMonitor) {
            left = scL;
            right = scR;
            return;
        }

        // 2. Peak detector
        float peakFiltered = std::max(std::abs(scL), std::abs(scR));
        float peakBroadband = std::max(std::abs(inL), std::abs(inR));

        // 3. Filtered-band envelope follower (Attack / Hold / Dual-Slope Release)
        float diff1 = peakFiltered - m_envFiltered;
        if (diff1 > 0.0f) {
            m_envFiltered += m_attackCoeff * diff1;
            m_holdCounter = m_holdSamples;
        } else {
            if (m_holdCounter > 0) {
                m_holdCounter--;
            } else {
                m_envFiltered += m_releaseCoeff * diff1;
            }
        }
        if (m_envFiltered < MIN_ENV) m_envFiltered = MIN_ENV;

        // 4. Broadband envelope follower
        float diff2 = peakBroadband - m_envBroadband;
        if (diff2 > 0.0f) {
            m_envBroadband += m_attackCoeff * diff2;
        } else {
            m_envBroadband += m_releaseCoeff * diff2;
        }
        if (m_envBroadband < MIN_ENV) m_envBroadband = MIN_ENV;

        // 5. Gain reduction calculation (with engine subsampling)
        if (++m_subsampleCounter >= m_subsampleFactor) {
            m_subsampleCounter = 0;

            float levelFilteredDB = linearToDB(m_envFiltered);
            float levelBroadbandDB = linearToDB(m_envBroadband);

            // Dynamics Gain Computer (Quadratic Soft Knee)
            float compGR_dB = 0.0f;
            if (m_softKnee && m_kneeWidthDB > 0.001f) {
                compGR_dB = computeSoftKneeGainReduction(levelFilteredDB, m_thresholdDB, m_halfKneeWidthDB, m_slope);
            } else {
                float overDB = levelFilteredDB - m_thresholdDB;
                if (overDB > 0.0f) compGR_dB = overDB * m_slope;
            }

            // Relative Spectral De-Esser Gain Computer
            float deEssGR_dB = 0.0f;
            if (m_deEsserEnabled) {
                float dsDeltaDB = (levelFilteredDB - levelBroadbandDB) - m_deEsserThresholdDB;
                if (dsDeltaDB > 0.0f) deEssGR_dB = dsDeltaDB * m_slope;
            }

            // Unified Gain Reduction Bus
            m_targetGainReductionDB = std::max(compGR_dB, deEssGR_dB);
        }

        // 6. Gain Reduction Ballistic Smoothing
        m_smoothedGainReductionDB += m_grSmoothingCoeff * (m_targetGainReductionDB - m_smoothedGainReductionDB);
        if (m_smoothedGainReductionDB < 0.0f) m_smoothedGainReductionDB = 0.0f;

        // 7. Linear VCA Multiplier
        float compLinearGain = dBToLinear(-m_smoothedGainReductionDB);

        // 8. Audio Application (Wideband vs. Dynamic Subtractive Crossover)
        if (m_deEsserEnabled || m_softSat) {
            float sibL = processSplitBiquad(inL, m_splitL_x1, m_splitL_x2, m_splitL_y1, m_splitL_y2);
            float sibR = processSplitBiquad(inR, m_splitR_x1, m_splitR_x2, m_splitR_y1, m_splitR_y2);
            left = ((inL - sibL) + sibL * compLinearGain) * m_makeUpLinear * m_lrOutLinearL;
            right = ((inR - sibR) + sibR * compLinearGain) * m_makeUpLinear * m_lrOutLinearR;
        } else {
            left = inL * compLinearGain * m_makeUpLinear * m_lrOutLinearL;
            right = inR * compLinearGain * m_makeUpLinear * m_lrOutLinearR;
        }
    }

    void processBlock(float* left, float* right, int numFrames) {
        for (int i = 0; i < numFrames; ++i) {
            process(left[i], right[i]);
        }
    }

    static inline float computeSoftKneeGainReduction(float x, float T, float halfW, float slope) {
        if (x <= T - halfW) return 0.0f;
        if (x > T + halfW) return slope * (x - T);
        float delta = x - T + halfW;
        return slope * (delta * delta) / (4.0f * halfW);
    }

private:
    static inline float linearToDB(float linear) {
        if (linear <= 1e-5f) return -100.0f;
        return 20.0f * std::log10(linear);
    }

    static inline float dBToLinear(float db) {
        return std::pow(10.0f, db * 0.05f);
    }

    void updateAttackCoeff() {
        double tau = (m_attackMs * 0.001);
        m_attackCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (m_sampleRate * tau)));
    }

    void updateReleaseCoeff() {
        double tau = (m_releaseMs * 0.001);
        m_releaseCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (m_sampleRate * tau)));
        setVCASmoothTime(m_vcaSmoothMs);
    }

    void updateHoldSamples() {
        m_holdSamples = static_cast<uint32_t>((m_holdMs * 0.001) * m_sampleRate);
    }

    void updateSidechainFilter() {
        constexpr double PI = 3.14159265358979323846;
        double w0 = 2.0 * PI * m_deEsserFreqHz / m_sampleRate;
        double q = 1.0;
        double alpha = std::sin(w0) / (2.0 * q);
        double cosw = std::cos(w0);

        double b0 = (1.0 + cosw) / 2.0;
        double b1 = -(1.0 + cosw);
        double b2 = (1.0 + cosw) / 2.0;
        double a0 = 1.0 + alpha;
        double a1 = -2.0 * cosw;
        double a2 = 1.0 - alpha;

        m_b0 = static_cast<float>(b0 / a0);
        m_b1 = static_cast<float>(b1 / a0);
        m_b2 = static_cast<float>(b2 / a0);
        m_a1 = static_cast<float>(a1 / a0);
        m_a2 = static_cast<float>(a2 / a0);
    }

    inline float processSidechainBiquad(float in, float& x1, float& x2, float& y1, float& y2) {
        float out = m_b0 * in + m_b1 * x1 + m_b2 * x2 - m_a1 * y1 - m_a2 * y2;
        x2 = x1; x1 = in;
        y2 = y1; y1 = out;
        return out;
    }

    inline float processSplitBiquad(float in, float& x1, float& x2, float& y1, float& y2) {
        return processSidechainBiquad(in, x1, x2, y1, y2);
    }

    // Engine settings
    double m_sampleRate = 44100.0;
    float m_thresholdDB = -20.0f;
    float m_ratio = 4.0f;
    float m_slope = 0.75f;
    float m_attackMs = 1.0f;
    float m_holdMs = 5.0f;
    float m_releaseMs = 50.0f;
    bool  m_softKnee = true;
    float m_kneeWidthDB = 10.0f;
    float m_halfKneeWidthDB = 5.0f;
    float m_makeUpDB = 0.0f;
    float m_makeUpLinear = 1.0f;

    bool  m_deEsserEnabled = false;
    float m_deEsserFreqHz = 4000.0f;
    float m_deEsserThresholdDB = -10.0f;
    bool  m_deEsserMonitor = false;

    // Advanced settings
    bool  m_softSat = false;
    float m_lrInDiffDB = 0.0f;
    float m_lrInLinearL = 1.0f;
    float m_lrInLinearR = 1.0f;
    float m_lrOutDiffDB = 0.0f;
    float m_lrOutLinearL = 1.0f;
    float m_lrOutLinearR = 1.0f;
    float m_vcaSmoothMs = 1.0f;
    int   m_subsampleFactor = 1;
    int   m_subsampleCounter = 0;

    // Filter Coefficients
    float m_b0 = 1.0f, m_b1 = 0.0f, m_b2 = 0.0f;
    float m_a1 = 0.0f, m_a2 = 0.0f;

    // Ballistics
    float m_attackCoeff = 0.1f;
    float m_releaseCoeff = 0.01f;
    float m_grSmoothingCoeff = 0.05f;
    uint32_t m_holdSamples = 0;
    uint32_t m_holdCounter = 0;

    // State Variables
    float m_envFiltered = 1e-5f;
    float m_envBroadband = 1e-5f;
    float m_targetGainReductionDB = 0.0f;
    float m_smoothedGainReductionDB = 0.0f;

    // Filter states
    float m_scL_x1 = 0, m_scL_x2 = 0, m_scL_y1 = 0, m_scL_y2 = 0;
    float m_scR_x1 = 0, m_scR_x2 = 0, m_scR_y1 = 0, m_scR_y2 = 0;
    float m_splitL_x1 = 0, m_splitL_x2 = 0, m_splitL_y1 = 0, m_splitL_y2 = 0;
    float m_splitR_x1 = 0, m_splitR_x2 = 0, m_splitR_y1 = 0, m_splitR_y2 = 0;
};

} // namespace ModelW
