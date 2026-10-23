#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <atomic>
#include "ModelW_DSP.h"

class ModelWAudioProcessor : public juce::AudioProcessor {
public:
    ModelWAudioProcessor();
    ~ModelWAudioProcessor() override;

    // =========================================================================
    // AudioProcessor Interface
    // =========================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // =========================================================================
    // Editor & Metadata
    // =========================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "ModelW"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    // =========================================================================
    // Programs & Presets
    // =========================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    // =========================================================================
    // Persistence State
    // =========================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // =========================================================================
    // Parameter Access & Metering
    // =========================================================================
    juce::AudioProcessorValueTreeState& getAPVTS() { return m_apvts; }

    float getMeterInL() const { return m_meterInL.load(std::memory_order_relaxed); }
    float getMeterInR() const { return m_meterInR.load(std::memory_order_relaxed); }
    float getMeterGR() const { return m_meterGR.load(std::memory_order_relaxed); }
    float getMeterOutL() const { return m_meterOutL.load(std::memory_order_relaxed); }
    float getMeterOutR() const { return m_meterOutR.load(std::memory_order_relaxed); }
    float getCurrentInputDB() const { return m_currentInputDB.load(std::memory_order_relaxed); }
    float getCurrentOutputDB() const { return m_currentOutputDB.load(std::memory_order_relaxed); }

    // Parameter ID Constants
    static constexpr const char* ID_THRESHOLD        = "threshold";
    static constexpr const char* ID_RATIO            = "ratio";
    static constexpr const char* ID_ATTACK           = "attack";
    static constexpr const char* ID_HOLD             = "hold";
    static constexpr const char* ID_RELEASE          = "release";
    static constexpr const char* ID_SOFT_KNEE        = "soft_knee";
    static constexpr const char* ID_KNEE_WIDTH       = "knee_width";
    static constexpr const char* ID_MAKEUP           = "makeup";

    static constexpr const char* ID_DEESSER_ENABLE   = "deesser_enable";
    static constexpr const char* ID_DEESSER_FREQ     = "deesser_freq";
    static constexpr const char* ID_DEESSER_THRESH   = "deesser_thresh";
    static constexpr const char* ID_DEESSER_MONITOR  = "deesser_monitor";

    static constexpr const char* ID_SOFTSAT          = "softsat";
    static constexpr const char* ID_LR_IN_DIFF       = "lr_in_diff";
    static constexpr const char* ID_LR_OUT_DIFF      = "lr_out_diff";
    static constexpr const char* ID_VCA_SMOOTH       = "vca_smooth";
    static constexpr const char* ID_SUBSAMPLE        = "subsample";
    static constexpr const char* ID_BYPASS           = "bypass";

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void updateEngineParameters();

    juce::AudioProcessorValueTreeState m_apvts;
    ModelW::Engine m_dspEngine;

    // Real-time Visual Metering (Thread-safe)
    std::atomic<float> m_meterInL{ -100.0f };
    std::atomic<float> m_meterInR{ -100.0f };
    std::atomic<float> m_meterGR{ 0.0f };
    std::atomic<float> m_meterOutL{ -100.0f };
    std::atomic<float> m_meterOutR{ -100.0f };
    std::atomic<float> m_currentInputDB{ -100.0f };
    std::atomic<float> m_currentOutputDB{ -100.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModelWAudioProcessor)
};
