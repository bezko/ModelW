#include "PluginEditor.h"

// =============================================================================
// LookAndFeel Implementation
// =============================================================================
ModelWLookAndFeel::ModelWLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, juce::Colour(0xff090d16));
    setColour(juce::Label::textColourId, juce::Colour(0xffcbd5e1));
    setColour(juce::TextButton::buttonColourId, juce::Colour(0xff182234));
    setColour(juce::TextButton::buttonOnColourId, juce::Colour(0xff0284c7));
    setColour(juce::TextButton::textColourOffId, juce::Colour(0xff94a3b8));
    setColour(juce::TextButton::textColourOnId, juce::Colour(0xffffffff));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff141b2d));
    setColour(juce::ComboBox::outlineColourId, juce::Colour(0xff334155));
    setColour(juce::ComboBox::textColourId, juce::Colour(0xffe2e8f0));
}

void ModelWLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                         float sliderPosProportional, float rotaryStartAngle,
                                         float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    float diameter = juce::jmin(bounds.getWidth(), bounds.getHeight()) - 22.0f;
    float radius = diameter * 0.5f;
    float centerX = bounds.getCentreX();
    float centerY = bounds.getY() + radius + 4.0f;

    // Track arc
    float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    float trackThickness = 4.5f;

    juce::Path backgroundArc;
    backgroundArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour(juce::Colour(0xff1e293b));
    g.strokePath(backgroundArc, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Value active arc
    if (sliderPosProportional > 0.001f) {
        juce::Path valueArc;
        valueArc.addCentredArc(centerX, centerY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        
        juce::Colour activeColor = juce::Colour(0xff38bdf8); // Cyan
        if (slider.getName().containsIgnoreCase("De-Esser")) activeColor = juce::Colour(0xffec4899); // Pink/Rose
        if (slider.getName().containsIgnoreCase("Trim") || slider.getName().containsIgnoreCase("Diff")) activeColor = juce::Colour(0xfff59e0b); // Amber

        g.setColour(activeColor);
        g.strokePath(valueArc, juce::PathStrokeType(trackThickness, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Dial body
    float innerRadius = radius - 8.0f;
    juce::ColourGradient dialGrad(juce::Colour(0xff1e293b), centerX - innerRadius, centerY - innerRadius,
                                  juce::Colour(0xff0f172a), centerX + innerRadius, centerY + innerRadius, false);
    g.setGradientFill(dialGrad);
    g.fillEllipse(centerX - innerRadius, centerY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);

    // Dial rim
    g.setColour(juce::Colour(0xff334155));
    g.drawEllipse(centerX - innerRadius, centerY - innerRadius, innerRadius * 2.0f, innerRadius * 2.0f, 1.2f);

    // Indicator pointer dot
    float pointerDist = innerRadius * 0.65f;
    float px = centerX + pointerDist * std::sin(angle);
    float py = centerY - pointerDist * std::cos(angle);
    g.setColour(juce::Colour(0xfff8fafc));
    g.fillEllipse(px - 2.5f, py - 2.5f, 5.0f, 5.0f);

    // Value text box at the bottom
    juce::Rectangle<int> textBounds(static_cast<int>(bounds.getX()),
                                   static_cast<int>(bounds.getBottom() - 18.0f),
                                   static_cast<int>(bounds.getWidth()), 16);
    g.setColour(juce::Colour(0xff0f172a));
    g.fillRoundedRectangle(textBounds.toFloat().reduced(4.0f, 0.0f), 3.0f);
    g.setColour(juce::Colour(0xff334155));
    g.drawRoundedRectangle(textBounds.toFloat().reduced(4.0f, 0.0f), 3.0f, 1.0f);

    g.setColour(juce::Colour(0xffe2e8f0));
    g.setFont(juce::FontOptions(11.0f).withStyle("SemiBold"));
    g.drawFittedText(slider.getTextFromValue(slider.getValue()), textBounds, juce::Justification::centred, 1);
}

void ModelWLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                         bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
{
    auto bounds = button.getLocalBounds().toFloat();
    float switchH = 16.0f;
    float switchW = 32.0f;
    auto switchArea = juce::Rectangle<float>(bounds.getX(), bounds.getCentreY() - switchH * 0.5f, switchW, switchH);

    bool isOn = button.getToggleState();

    // Switch track
    juce::Colour trackCol = isOn ? juce::Colour(0xff0284c7) : juce::Colour(0xff1e293b);
    g.setColour(trackCol);
    g.fillRoundedRectangle(switchArea, switchH * 0.5f);
    g.setColour(isOn ? juce::Colour(0xff38bdf8) : juce::Colour(0xff475569));
    g.drawRoundedRectangle(switchArea, switchH * 0.5f, 1.0f);

    // Switch thumb
    float thumbPadding = 2.0f;
    float thumbDiameter = switchH - thumbPadding * 2.0f;
    float thumbX = isOn ? (switchArea.getRight() - thumbDiameter - thumbPadding) : (switchArea.getX() + thumbPadding);
    float thumbY = switchArea.getY() + thumbPadding;

    g.setColour(juce::Colour(0xfff8fafc));
    g.fillEllipse(thumbX, thumbY, thumbDiameter, thumbDiameter);

    // Label text
    auto textArea = bounds.withTrimmedLeft(switchW + 8.0f);
    g.setColour(isOn ? juce::Colour(0xfff1f5f9) : juce::Colour(0xff94a3b8));
    g.setFont(juce::FontOptions(12.0f).withStyle(isOn ? "Bold" : "Regular"));
    g.drawFittedText(button.getButtonText(), textArea.toNearestInt(), juce::Justification::centredLeft, 1);
}

void ModelWLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool /*isButtonDown*/,
                                     int /*buttonX*/, int /*buttonY*/, int /*buttonW*/, int /*buttonH*/, juce::ComboBox& /*box*/)
{
    auto bounds = juce::Rectangle<float>(0, 0, (float)width, (float)height);
    g.setColour(juce::Colour(0xff141b2d));
    g.fillRoundedRectangle(bounds, 4.0f);
    g.setColour(juce::Colour(0xff334155));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // Dropdown arrow
    juce::Path arrow;
    float arrowX = width - 16.0f;
    float arrowY = height * 0.5f - 2.0f;
    arrow.addTriangle(arrowX, arrowY, arrowX + 8.0f, arrowY, arrowX + 4.0f, arrowY + 5.0f);
    g.setColour(juce::Colour(0xff94a3b8));
    g.fillPath(arrow);
}

// =============================================================================
// TransferCurveComponent Implementation
// =============================================================================
TransferCurveComponent::TransferCurveComponent(ModelWAudioProcessor& proc)
    : m_processor(proc)
{
}

void TransferCurveComponent::updateCurve(float thresh, float ratio, bool softKnee, float kneeWidth, float makeup, float inDB, float outDB)
{
    m_threshold = thresh;
    m_ratio = ratio;
    m_softKnee = softKnee;
    m_kneeWidth = kneeWidth;
    m_makeup = makeup;
    m_inputDB = inDB;
    m_outputDB = outDB;
    repaint();
}

void TransferCurveComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);
    
    // Background canvas
    g.setColour(juce::Colour(0xff0b101b));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff1e293b));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    const float minDB = -60.0f;
    const float maxDB = 0.0f;
    const float dbSpan = maxDB - minDB;

    auto mapX = [&](float db) {
        float norm = (db - minDB) / dbSpan;
        return bounds.getX() + norm * bounds.getWidth();
    };

    auto mapY = [&](float db) {
        float norm = (db - minDB) / dbSpan;
        return bounds.getBottom() - norm * bounds.getHeight();
    };

    // Draw Grid Lines (-40, -20, 0 dB)
    g.setFont(juce::FontOptions(9.0f));
    const float gridLines[] = { -40.0f, -20.0f, 0.0f };
    for (float dbVal : gridLines) {
        float x = mapX(dbVal);
        float y = mapY(dbVal);

        g.setColour(juce::Colour(0xff1e293b));
        g.drawHorizontalLine(static_cast<int>(y), bounds.getX(), bounds.getRight());
        g.drawVerticalLine(static_cast<int>(x), bounds.getY(), bounds.getBottom());

        g.setColour(juce::Colour(0xff475569));
        g.drawText(juce::String(static_cast<int>(dbVal)) + " dB",
                   juce::Rectangle<float>(x + 3.0f, bounds.getY() + 3.0f, 40.0f, 12.0f).toNearestInt(),
                   juce::Justification::topLeft, false);
    }

    // Unity diagonal reference line (Dashed)
    {
        juce::Path unityPath;
        unityPath.startNewSubPath(mapX(-60.0f), mapY(-60.0f));
        unityPath.lineTo(mapX(0.0f), mapY(0.0f));
        g.setColour(juce::Colour(0xff334155));
        juce::Path dashedPath;
        const float dashes[] = { 4.0f, 4.0f };
        juce::PathStrokeType stroke(1.0f);
        stroke.createDashedStroke(dashedPath, unityPath, dashes, 2);
        g.strokePath(dashedPath, stroke);
    }

    // Dynamic Compression Curve
    float slope = (m_ratio > 1.0f) ? (1.0f - 1.0f / m_ratio) : 0.0f;
    float halfW = m_kneeWidth * 0.5f;

    juce::Path curvePath;
    juce::Path fillPath;
    bool pathStarted = false;

    for (float inDB = minDB; inDB <= maxDB; inDB += 0.5f) {
        float compGR = 0.0f;
        if (m_softKnee && m_kneeWidth > 0.001f) {
            compGR = ModelW::Engine::computeSoftKneeGainReduction(inDB, m_threshold, halfW, slope);
        } else {
            float over = inDB - m_threshold;
            if (over > 0.0f) compGR = over * slope;
        }

        float outDB = inDB - compGR + m_makeup;
        if (outDB > 12.0f) outDB = 12.0f;

        float cx = mapX(inDB);
        float cy = mapY(std::clamp(outDB, minDB, 12.0f));

        if (!pathStarted) {
            curvePath.startNewSubPath(cx, cy);
            fillPath.startNewSubPath(cx, bounds.getBottom());
            fillPath.lineTo(cx, cy);
            pathStarted = true;
        } else {
            curvePath.lineTo(cx, cy);
            fillPath.lineTo(cx, cy);
        }
    }

    // Fill underneath curve
    fillPath.lineTo(bounds.getRight(), bounds.getBottom());
    fillPath.closeSubPath();
    juce::ColourGradient fillGrad(juce::Colour(0x3338bdf8), 0, bounds.getY(),
                                  juce::Colour(0x0538bdf8), 0, bounds.getBottom(), false);
    g.setGradientFill(fillGrad);
    g.fillPath(fillPath);

    // Stroke curve
    g.setColour(juce::Colour(0xff38bdf8));
    g.strokePath(curvePath, juce::PathStrokeType(2.5f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // Threshold indicator line
    float threshX = mapX(m_threshold);
    g.setColour(juce::Colour(0x66f59e0b)); // Amber low-opacity
    g.drawVerticalLine(static_cast<int>(threshX), bounds.getY(), bounds.getBottom());

    // Real-time Operating Point Tracker Dot
    if (m_inputDB > -58.0f) {
        float dotX = mapX(std::clamp(m_inputDB, minDB, maxDB));
        float dotY = mapY(std::clamp(m_outputDB, minDB, maxDB));

        // Glow outer
        g.setColour(juce::Colour(0x44fbbf24));
        g.fillEllipse(dotX - 7.0f, dotY - 7.0f, 14.0f, 14.0f);
        // Inner bright dot
        g.setColour(juce::Colour(0xfffbbf24));
        g.fillEllipse(dotX - 3.5f, dotY - 3.5f, 7.0f, 7.0f);
    }
}

// =============================================================================
// DeEsserSpectrumComponent Implementation
// =============================================================================
DeEsserSpectrumComponent::DeEsserSpectrumComponent()
{
}

void DeEsserSpectrumComponent::setParams(float freqHz, float thresholdDB, bool enabled, float currentGR)
{
    m_freqHz = freqHz;
    m_thresholdDB = thresholdDB;
    m_enabled = enabled;
    m_grDB = currentGR;
    repaint();
}

void DeEsserSpectrumComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(4.0f);

    g.setColour(juce::Colour(0xff0b101b));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(juce::Colour(0xff1e293b));
    g.drawRoundedRectangle(bounds, 6.0f, 1.0f);

    // Frequency markers (100 Hz, 1 kHz, 4 kHz, 10 kHz, 20 kHz)
    auto freqToX = [&](float f) {
        float norm = (std::log10(f) - std::log10(100.0f)) / (std::log10(20000.0f) - std::log10(100.0f));
        return bounds.getX() + std::clamp(norm, 0.0f, 1.0f) * bounds.getWidth();
    };

    const float testFreqs[] = { 100.0f, 1000.0f, 4000.0f, 10000.0f };
    const char* freqLabels[] = { "100Hz", "1kHz", "4kHz", "10kHz" };

    g.setFont(juce::FontOptions(9.0f));
    for (int i = 0; i < 4; ++i) {
        float x = freqToX(testFreqs[i]);
        g.setColour(juce::Colour(0xff1e293b));
        g.drawVerticalLine(static_cast<int>(x), bounds.getY(), bounds.getBottom());
        g.setColour(juce::Colour(0xff475569));
        g.drawText(freqLabels[i], juce::Rectangle<float>(x + 3.0f, bounds.getY() + 3.0f, 35.0f, 12.0f).toNearestInt(),
                   juce::Justification::topLeft, false);
    }

    // Sibilance Detection Region
    float splitX = freqToX(m_freqHz);
    auto sibRegion = bounds.withTrimmedLeft(splitX - bounds.getX());

    if (m_enabled) {
        juce::Colour sibCol = juce::Colour(0x22ec4899); // Pink glow
        g.setColour(sibCol);
        g.fillRect(sibRegion);

        // Active split line
        g.setColour(juce::Colour(0xffec4899));
        g.drawVerticalLine(static_cast<int>(splitX), bounds.getY(), bounds.getBottom());

        // Cutoff label
        g.setColour(juce::Colour(0xfff472b6));
        g.drawText("CUTOFF: " + juce::String(m_freqHz >= 1000.0f ? m_freqHz * 0.001f : m_freqHz, 1) + " kHz",
                   juce::Rectangle<float>(splitX + 6.0f, bounds.getY() + 20.0f, 100.0f, 14.0f).toNearestInt(),
                   juce::Justification::topLeft, false);

        // Attenuation Graphic in Sibilant region
        float dipNorm = std::clamp(m_grDB / 20.0f, 0.0f, 1.0f);
        float dipY = bounds.getCentreY() + dipNorm * (bounds.getHeight() * 0.4f);

        juce::Path sibilantCurve;
        sibilantCurve.startNewSubPath(bounds.getX(), bounds.getCentreY());
        sibilantCurve.lineTo(splitX, bounds.getCentreY());
        sibilantCurve.quadraticTo(splitX + (bounds.getRight() - splitX) * 0.4f, dipY, bounds.getRight(), dipY);

        g.setColour(juce::Colour(0xffec4899));
        g.strokePath(sibilantCurve, juce::PathStrokeType(2.0f));
    } else {
        g.setColour(juce::Colour(0xff64748b));
        g.drawText("DE-ESSER IS CURRENTLY BYPASSED", bounds.toNearestInt(), juce::Justification::centred, false);
    }
}

// =============================================================================
// ModelWAudioProcessorEditor Implementation
// =============================================================================
ModelWAudioProcessorEditor::ModelWAudioProcessorEditor(ModelWAudioProcessor& p)
    : AudioProcessorEditor(&p),
      m_processor(p),
      m_curveView(p)
{
    setLookAndFeel(&m_lookAndFeel);

    // Setup Header & Bypass
    m_bypassButton.setButtonText("BYPASS");
    m_bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_BYPASS, m_bypassButton);
    addAndMakeVisible(m_bypassButton);

    // Setup Tabs
    auto setupTabBtn = [this](juce::TextButton& btn, Tab tab) {
        btn.setClickingTogglesState(false);
        btn.onClick = [this, tab]() {
            m_currentTab = tab;
            updateTabVisibility();
            repaint();
        };
        addAndMakeVisible(btn);
    };

    setupTabBtn(m_tabDynamics, Tab::Dynamics);
    setupTabBtn(m_tabDeEsser, Tab::DeEsser);
    setupTabBtn(m_tabAdvanced, Tab::Advanced);

    // -------------------------------------------------------------------------
    // Dynamics Tab Controls
    // -------------------------------------------------------------------------
    addChildComponent(m_curveView);

    setupSlider(m_sliderThreshold, m_labelThreshold, "THRESHOLD", " dB");
    m_attachThreshold = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_THRESHOLD, m_sliderThreshold);

    setupSlider(m_sliderRatio, m_labelRatio, "RATIO", ":1");
    m_attachRatio = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_RATIO, m_sliderRatio);

    setupSlider(m_sliderAttack, m_labelAttack, "ATTACK", " ms");
    m_attachAttack = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_ATTACK, m_sliderAttack);

    setupSlider(m_sliderHold, m_labelHold, "HOLD", " ms");
    m_attachHold = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_HOLD, m_sliderHold);

    setupSlider(m_sliderRelease, m_labelRelease, "RELEASE", " ms");
    m_attachRelease = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_RELEASE, m_sliderRelease);

    setupToggle(m_toggleSoftKnee, "Soft Knee");
    m_attachSoftKnee = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_SOFT_KNEE, m_toggleSoftKnee);

    setupSlider(m_sliderKneeWidth, m_labelKneeWidth, "KNEE WIDTH", " dB");
    m_attachKneeWidth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_KNEE_WIDTH, m_sliderKneeWidth);

    setupSlider(m_sliderMakeup, m_labelMakeup, "MAKE-UP", " dB");
    m_attachMakeup = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_MAKEUP, m_sliderMakeup);

    // -------------------------------------------------------------------------
    // De-Esser Tab Controls
    // -------------------------------------------------------------------------
    addChildComponent(m_spectrumView);

    setupToggle(m_toggleDeEsserEnable, "Activate De-Esser");
    m_attachDeEsserEnable = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_DEESSER_ENABLE, m_toggleDeEsserEnable);

    setupToggle(m_toggleDeEsserMonitor, "Audition Sidechain Filter");
    m_attachDeEsserMonitor = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_DEESSER_MONITOR, m_toggleDeEsserMonitor);

    setupSlider(m_sliderDeEsserFreq, m_labelDeEsserFreq, "FREQUENCY", " Hz");
    m_sliderDeEsserFreq.setName("De-Esser Freq");
    m_attachDeEsserFreq = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_DEESSER_FREQ, m_sliderDeEsserFreq);

    setupSlider(m_sliderDeEsserThresh, m_labelDeEsserThresh, "REL THRESHOLD", " dB");
    m_sliderDeEsserThresh.setName("De-Esser Thresh");
    m_attachDeEsserThresh = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_DEESSER_THRESH, m_sliderDeEsserThresh);

    // -------------------------------------------------------------------------
    // Advanced Tab Controls
    // -------------------------------------------------------------------------
    setupToggle(m_toggleSoftSat, "SoftSat Crossover Harmonic Saturation");
    m_attachSoftSat = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_SOFTSAT, m_toggleSoftSat);

    setupSlider(m_sliderLRInDiff, m_labelLRInDiff, "L/R INPUT TRIM", " dB");
    m_sliderLRInDiff.setName("Trim In");
    m_attachLRInDiff = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_LR_IN_DIFF, m_sliderLRInDiff);

    setupSlider(m_sliderLROutDiff, m_labelLROutDiff, "L/R OUTPUT TRIM", " dB");
    m_sliderLROutDiff.setName("Trim Out");
    m_attachLROutDiff = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_LR_OUT_DIFF, m_sliderLROutDiff);

    setupSlider(m_sliderVCASmooth, m_labelVCASmooth, "VCA SMOOTHING", " ms");
    m_attachVCASmooth = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_VCA_SMOOTH, m_sliderVCASmooth);

    m_comboSubsample.addItemList(juce::StringArray{ "1x (Full Rate)", "2x Control Rate", "4x Control Rate", "8x Control Rate" }, 1);
    m_labelSubsample.setText("SUBSAMPLING FACTOR", juce::dontSendNotification);
    m_labelSubsample.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    m_labelSubsample.setJustificationType(juce::Justification::centred);
    addChildComponent(m_comboSubsample);
    addChildComponent(m_labelSubsample);
    m_attachSubsample = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        m_processor.getAPVTS(), ModelWAudioProcessor::ID_SUBSAMPLE, m_comboSubsample);

    // Initial state
    updateTabVisibility();

    // Fixed 560x560 square window
    setSize(560, 560);
    setResizable(false, false);

    startTimerHz(30);
}

ModelWAudioProcessorEditor::~ModelWAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

void ModelWAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
                                             const juce::String& text, const juce::String& /*suffix*/)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addChildComponent(slider);

    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(10.0f).withStyle("Bold"));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colour(0xff94a3b8));
    addChildComponent(label);
}

void ModelWAudioProcessorEditor::setupToggle(juce::ToggleButton& toggle, const juce::String& text)
{
    toggle.setButtonText(text);
    addChildComponent(toggle);
}

void ModelWAudioProcessorEditor::updateTabVisibility()
{
    bool isDyn = (m_currentTab == Tab::Dynamics);
    bool isDeEss = (m_currentTab == Tab::DeEsser);
    bool isAdv = (m_currentTab == Tab::Advanced);

    // Dynamics
    m_curveView.setVisible(isDyn);
    m_sliderThreshold.setVisible(isDyn);
    m_labelThreshold.setVisible(isDyn);
    m_sliderRatio.setVisible(isDyn);
    m_labelRatio.setVisible(isDyn);
    m_sliderAttack.setVisible(isDyn);
    m_labelAttack.setVisible(isDyn);
    m_sliderHold.setVisible(isDyn);
    m_labelHold.setVisible(isDyn);
    m_sliderRelease.setVisible(isDyn);
    m_labelRelease.setVisible(isDyn);
    m_toggleSoftKnee.setVisible(isDyn);
    m_sliderKneeWidth.setVisible(isDyn);
    m_labelKneeWidth.setVisible(isDyn);
    m_sliderMakeup.setVisible(isDyn);
    m_labelMakeup.setVisible(isDyn);

    // De-Esser
    m_spectrumView.setVisible(isDeEss);
    m_toggleDeEsserEnable.setVisible(isDeEss);
    m_toggleDeEsserMonitor.setVisible(isDeEss);
    m_sliderDeEsserFreq.setVisible(isDeEss);
    m_labelDeEsserFreq.setVisible(isDeEss);
    m_sliderDeEsserThresh.setVisible(isDeEss);
    m_labelDeEsserThresh.setVisible(isDeEss);

    // Advanced
    m_toggleSoftSat.setVisible(isAdv);
    m_sliderLRInDiff.setVisible(isAdv);
    m_labelLRInDiff.setVisible(isAdv);
    m_sliderLROutDiff.setVisible(isAdv);
    m_labelLROutDiff.setVisible(isAdv);
    m_sliderVCASmooth.setVisible(isAdv);
    m_labelVCASmooth.setVisible(isAdv);
    m_comboSubsample.setVisible(isAdv);
    m_labelSubsample.setVisible(isAdv);
}

void ModelWAudioProcessorEditor::timerCallback()
{
    // Smooth meters
    const float decay = 0.82f;

    float rawInL = m_processor.getMeterInL();
    float rawInR = m_processor.getMeterInR();
    float rawGR  = m_processor.getMeterGR();
    float rawOutL = m_processor.getMeterOutL();
    float rawOutR = m_processor.getMeterOutR();

    m_smoothInL = std::max(rawInL, m_smoothInL * decay + rawInL * (1.0f - decay));
    m_smoothInR = std::max(rawInR, m_smoothInR * decay + rawInR * (1.0f - decay));
    m_smoothGR  = std::max(rawGR, m_smoothGR * decay + rawGR * (1.0f - decay));
    m_smoothOutL = std::max(rawOutL, m_smoothOutL * decay + rawOutL * (1.0f - decay));
    m_smoothOutR = std::max(rawOutR, m_smoothOutR * decay + rawOutR * (1.0f - decay));

    if (m_currentTab == Tab::Dynamics) {
        float thresh = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_THRESHOLD)->load();
        float ratio = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_RATIO)->load();
        bool softKnee = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_SOFT_KNEE)->load() > 0.5f;
        float kneeWidth = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_KNEE_WIDTH)->load();
        float makeup = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_MAKEUP)->load();

        m_curveView.updateCurve(thresh, ratio, softKnee, kneeWidth, makeup,
                                m_processor.getCurrentInputDB(), m_processor.getCurrentOutputDB());
    } else if (m_currentTab == Tab::DeEsser) {
        float freq = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_DEESSER_FREQ)->load();
        float thresh = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_DEESSER_THRESH)->load();
        bool enabled = m_processor.getAPVTS().getRawParameterValue(ModelWAudioProcessor::ID_DEESSER_ENABLE)->load() > 0.5f;

        m_spectrumView.setParams(freq, thresh, enabled, m_smoothGR);
    }

    repaint(0, 480, getWidth(), 80); // Repaint metering bar at bottom
}

void ModelWAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient bgGrad(juce::Colour(0xff090d16), 0, 0,
                               juce::Colour(0xff111827), 0, (float)getHeight(), false);
    g.setGradientFill(bgGrad);
    g.fillAll();

    // Top Header
    g.setFont(juce::FontOptions(18.0f).withStyle("Bold"));
    g.setColour(juce::Colour(0xfff8fafc));
    g.drawText("MODEL W", 18, 10, 100, 24, juce::Justification::left, false);

    g.setFont(juce::FontOptions(9.5f).withStyle("SemiBold"));
    g.setColour(juce::Colour(0xff38bdf8));
    g.drawText("PRECISION DYNAMICS PROCESSOR", 108, 14, 200, 20, juce::Justification::left, false);

    // Tab Bar Background & Active Indicator
    g.setColour(juce::Colour(0xff0f172a));
    g.fillRect(0, 44, getWidth(), 34);
    g.setColour(juce::Colour(0xff1e293b));
    g.drawHorizontalLine(44, 0.0f, (float)getWidth());
    g.drawHorizontalLine(78, 0.0f, (float)getWidth());

    // Highlight active tab indicator
    int tabW = 100;
    int tabX = 14 + (m_currentTab == Tab::Dynamics ? 0 : (m_currentTab == Tab::DeEsser ? tabW + 6 : (tabW + 6) * 2));
    g.setColour(juce::Colour(0xff38bdf8));
    g.fillRect(tabX, 75, tabW, 3);

    // -------------------------------------------------------------------------
    // Footer Metering Bar (y: 485, height: 70)
    // -------------------------------------------------------------------------
    g.setColour(juce::Colour(0xff0b0f19));
    g.fillRect(0, 480, getWidth(), 80);
    g.setColour(juce::Colour(0xff1e293b));
    g.drawHorizontalLine(480, 0.0f, (float)getWidth());

    auto drawMeter = [&](int mx, int my, int mw, int mh, float valDB, const juce::Colour& barCol, const juce::String& name) {
        g.setColour(juce::Colour(0xff111827));
        g.fillRoundedRectangle((float)mx, (float)my, (float)mw, (float)mh, 2.0f);

        float norm = std::clamp((valDB + 60.0f) / 60.0f, 0.0f, 1.0f);
        int fillH = static_cast<int>(norm * mh);

        if (fillH > 0) {
            g.setColour(barCol);
            g.fillRoundedRectangle((float)mx, (float)(my + mh - fillH), (float)mw, (float)fillH, 2.0f);
        }

        g.setFont(juce::FontOptions(8.5f));
        g.setColour(juce::Colour(0xff64748b));
        g.drawText(name, mx - 4, my + mh + 2, mw + 8, 12, juce::Justification::centred, false);
    };

    auto drawGRMeter = [&](int mx, int my, int mw, int mh, float grDB) {
        g.setColour(juce::Colour(0xff111827));
        g.fillRoundedRectangle((float)mx, (float)my, (float)mw, (float)mh, 2.0f);

        float norm = std::clamp(grDB / 30.0f, 0.0f, 1.0f);
        int fillH = static_cast<int>(norm * mh);

        if (fillH > 0) {
            g.setColour(juce::Colour(0xfff59e0b)); // Amber GR
            g.fillRoundedRectangle((float)mx, (float)my, (float)mw, (float)fillH, 2.0f);
        }

        g.setFont(juce::FontOptions(8.5f));
        g.setColour(juce::Colour(0xff64748b));
        g.drawText("GR", mx - 4, my + mh + 2, mw + 8, 12, juce::Justification::centred, false);
    };

    // Input Meters
    drawMeter(40, 488, 8, 48, m_smoothInL, juce::Colour(0xff10b981), "IN L");
    drawMeter(52, 488, 8, 48, m_smoothInR, juce::Colour(0xff10b981), "IN R");

    // GR Meter
    drawGRMeter(274, 488, 12, 48, m_smoothGR);

    // Output Meters
    drawMeter(496, 488, 8, 48, m_smoothOutL, juce::Colour(0xff06b6d4), "OUT L");
    drawMeter(508, 488, 8, 48, m_smoothOutR, juce::Colour(0xff06b6d4), "OUT R");

    // Meter value readouts
    g.setFont(juce::FontOptions(10.0f).withStyle("SemiBold"));
    g.setColour(juce::Colour(0xffcbd5e1));
    g.drawText("IN: " + juce::String(std::max(m_smoothInL, m_smoothInR), 1) + " dB", 75, 502, 100, 16, juce::Justification::left, false);
    g.drawText("GR: -" + juce::String(m_smoothGR, 1) + " dB", 298, 502, 100, 16, juce::Justification::left, false);
    g.drawText("OUT: " + juce::String(std::max(m_smoothOutL, m_smoothOutR), 1) + " dB", 385, 502, 100, 16, juce::Justification::right, false);
}

void ModelWAudioProcessorEditor::resized()
{
    // Header
    m_bypassButton.setBounds(getWidth() - 95, 12, 85, 24);

    // Tab Buttons
    int tabW = 100;
    int tabH = 26;
    m_tabDynamics.setBounds(14, 48, tabW, tabH);
    m_tabDeEsser.setBounds(14 + tabW + 6, 48, tabW, tabH);
    m_tabAdvanced.setBounds(14 + (tabW + 6) * 2, 48, tabW, tabH);

    // -------------------------------------------------------------------------
    // Dynamics Tab Layout
    // -------------------------------------------------------------------------
    m_curveView.setBounds(16, 88, getWidth() - 32, 140);

    int knobW = 75;
    int knobH = 92;
    int row1Y = 240;
    int colSpacing = 132;
    int startX = 28;

    // Row 1: Threshold, Ratio, Attack, Release
    m_sliderThreshold.setBounds(startX, row1Y, knobW, knobH);
    m_labelThreshold.setBounds(startX - 10, row1Y - 14, knobW + 20, 14);

    m_sliderRatio.setBounds(startX + colSpacing, row1Y, knobW, knobH);
    m_labelRatio.setBounds(startX + colSpacing - 10, row1Y - 14, knobW + 20, 14);

    m_sliderAttack.setBounds(startX + colSpacing * 2, row1Y, knobW, knobH);
    m_labelAttack.setBounds(startX + colSpacing * 2 - 10, row1Y - 14, knobW + 20, 14);

    m_sliderRelease.setBounds(startX + colSpacing * 3, row1Y, knobW, knobH);
    m_labelRelease.setBounds(startX + colSpacing * 3 - 10, row1Y - 14, knobW + 20, 14);

    // Row 2: Hold, Soft Knee, Knee Width, Makeup
    int row2Y = 352;
    m_sliderHold.setBounds(startX, row2Y, knobW, knobH);
    m_labelHold.setBounds(startX - 10, row2Y - 14, knobW + 20, 14);

    m_toggleSoftKnee.setBounds(startX + colSpacing - 10, row2Y + 24, 110, 24);

    m_sliderKneeWidth.setBounds(startX + colSpacing * 2, row2Y, knobW, knobH);
    m_labelKneeWidth.setBounds(startX + colSpacing * 2 - 10, row2Y - 14, knobW + 20, 14);

    m_sliderMakeup.setBounds(startX + colSpacing * 3, row2Y, knobW, knobH);
    m_labelMakeup.setBounds(startX + colSpacing * 3 - 10, row2Y - 14, knobW + 20, 14);

    // -------------------------------------------------------------------------
    // De-Esser Tab Layout
    // -------------------------------------------------------------------------
    m_spectrumView.setBounds(16, 88, getWidth() - 32, 170);

    int deEssCtrlsY = 280;
    m_toggleDeEsserEnable.setBounds(32, deEssCtrlsY, 180, 26);
    m_toggleDeEsserMonitor.setBounds(32, deEssCtrlsY + 40, 200, 26);

    int dsKnobW = 90;
    int dsKnobH = 110;
    m_sliderDeEsserFreq.setBounds(270, deEssCtrlsY - 10, dsKnobW, dsKnobH);
    m_labelDeEsserFreq.setBounds(260, deEssCtrlsY - 26, dsKnobW + 20, 14);

    m_sliderDeEsserThresh.setBounds(410, deEssCtrlsY - 10, dsKnobW, dsKnobH);
    m_labelDeEsserThresh.setBounds(400, deEssCtrlsY - 26, dsKnobW + 20, 14);

    // -------------------------------------------------------------------------
    // Advanced Tab Layout
    // -------------------------------------------------------------------------
    int advY = 100;
    m_toggleSoftSat.setBounds(32, advY, 320, 26);

    int advRow2Y = 155;
    m_sliderLRInDiff.setBounds(60, advRow2Y, 90, 105);
    m_labelLRInDiff.setBounds(40, advRow2Y - 16, 130, 14);

    m_sliderLROutDiff.setBounds(220, advRow2Y, 90, 105);
    m_labelLROutDiff.setBounds(200, advRow2Y - 16, 130, 14);

    m_sliderVCASmooth.setBounds(380, advRow2Y, 90, 105);
    m_labelVCASmooth.setBounds(360, advRow2Y - 16, 130, 14);

    int advRow3Y = 300;
    m_labelSubsample.setBounds(60, advRow3Y, 200, 16);
    m_comboSubsample.setBounds(60, advRow3Y + 22, 220, 28);
}
