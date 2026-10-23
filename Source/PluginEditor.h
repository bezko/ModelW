#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

// =============================================================================
// Custom Modern Look & Feel for ModelW
// =============================================================================
class ModelWLookAndFeel : public juce::LookAndFeel_V4 {
public:
    ModelWLookAndFeel();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider& slider) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                          bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override;
};

// =============================================================================
// Real-time Dynamic Transfer Function Curve Component
// =============================================================================
class TransferCurveComponent : public juce::Component {
public:
    TransferCurveComponent(ModelWAudioProcessor& proc);

    void paint(juce::Graphics& g) override;
    void updateCurve(float thresh, float ratio, bool softKnee, float kneeWidth, float makeup, float inDB, float outDB);

private:
    ModelWAudioProcessor& m_processor;
    float m_threshold = -20.0f;
    float m_ratio = 4.0f;
    bool  m_softKnee = true;
    float m_kneeWidth = 10.0f;
    float m_makeup = 0.0f;
    float m_inputDB = -100.0f;
    float m_outputDB = -100.0f;
};

// =============================================================================
// Spectral Display Component for De-Esser Tab
// =============================================================================
class DeEsserSpectrumComponent : public juce::Component {
public:
    DeEsserSpectrumComponent();

    void paint(juce::Graphics& g) override;
    void setParams(float freqHz, float thresholdDB, bool enabled, float currentGR);

private:
    float m_freqHz = 4000.0f;
    float m_thresholdDB = -10.0f;
    bool  m_enabled = false;
    float m_grDB = 0.0f;
};

// =============================================================================
// Main AudioProcessorEditor for ModelW
// =============================================================================
class ModelWAudioProcessorEditor : public juce::AudioProcessorEditor,
                                   public juce::Timer
{
public:
    explicit ModelWAudioProcessorEditor(ModelWAudioProcessor&);
    ~ModelWAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text,
                     const juce::String& suffix = "");
    void setupToggle(juce::ToggleButton& toggle, const juce::String& text);

    ModelWAudioProcessor& m_processor;
    ModelWLookAndFeel m_lookAndFeel;

    // Header Controls
    juce::ToggleButton m_bypassButton{ "BYPASS" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_bypassAttachment;

    // Tab Navigation
    enum class Tab { Dynamics, DeEsser, Advanced };
    Tab m_currentTab = Tab::Dynamics;
    juce::TextButton m_tabDynamics{ "DYNAMICS" };
    juce::TextButton m_tabDeEsser{ "DE-ESSER" };
    juce::TextButton m_tabAdvanced{ "ADVANCED" };

    // -------------------------------------------------------------------------
    // Dynamics Tab Components
    // -------------------------------------------------------------------------
    TransferCurveComponent m_curveView;

    juce::Slider m_sliderThreshold;
    juce::Label  m_labelThreshold;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachThreshold;

    juce::Slider m_sliderRatio;
    juce::Label  m_labelRatio;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachRatio;

    juce::Slider m_sliderAttack;
    juce::Label  m_labelAttack;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachAttack;

    juce::Slider m_sliderHold;
    juce::Label  m_labelHold;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachHold;

    juce::Slider m_sliderRelease;
    juce::Label  m_labelRelease;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachRelease;

    juce::ToggleButton m_toggleSoftKnee{ "Soft Knee" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_attachSoftKnee;

    juce::Slider m_sliderKneeWidth;
    juce::Label  m_labelKneeWidth;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachKneeWidth;

    juce::Slider m_sliderMakeup;
    juce::Label  m_labelMakeup;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachMakeup;

    // -------------------------------------------------------------------------
    // De-Esser Tab Components
    // -------------------------------------------------------------------------
    DeEsserSpectrumComponent m_spectrumView;

    juce::ToggleButton m_toggleDeEsserEnable{ "Enable De-Esser" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_attachDeEsserEnable;

    juce::ToggleButton m_toggleDeEsserMonitor{ "Listen / Sidechain Monitor" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_attachDeEsserMonitor;

    juce::Slider m_sliderDeEsserFreq;
    juce::Label  m_labelDeEsserFreq;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachDeEsserFreq;

    juce::Slider m_sliderDeEsserThresh;
    juce::Label  m_labelDeEsserThresh;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachDeEsserThresh;

    // -------------------------------------------------------------------------
    // Advanced Tab Components
    // -------------------------------------------------------------------------
    juce::ToggleButton m_toggleSoftSat{ "SoftSat Crossover Warmth" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> m_attachSoftSat;

    juce::Slider m_sliderLRInDiff;
    juce::Label  m_labelLRInDiff;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachLRInDiff;

    juce::Slider m_sliderLROutDiff;
    juce::Label  m_labelLROutDiff;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachLROutDiff;

    juce::Slider m_sliderVCASmooth;
    juce::Label  m_labelVCASmooth;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_attachVCASmooth;

    juce::ComboBox m_comboSubsample;
    juce::Label    m_labelSubsample;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> m_attachSubsample;

    // Metering Bar variables
    float m_smoothInL = -100.0f;
    float m_smoothInR = -100.0f;
    float m_smoothGR  = 0.0f;
    float m_smoothOutL = -100.0f;
    float m_smoothOutR = -100.0f;

    void updateTabVisibility();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModelWAudioProcessorEditor)
};
