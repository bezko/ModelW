#include "PluginProcessor.h"
#include "PluginEditor.h"

ModelWAudioProcessor::ModelWAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      m_apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

ModelWAudioProcessor::~ModelWAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout ModelWAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    auto makeFloat = [](const char* id, const juce::String& name, juce::NormalisableRange<float> range, float defVal,
                        std::function<juce::String(float, int)> strFunc) {
        return std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID{ id, 1 },
            name,
            range,
            defVal,
            juce::AudioParameterFloatAttributes().withStringFromValueFunction(std::move(strFunc)));
    };

    // Dynamics Parameters
    params.push_back(makeFloat(ID_THRESHOLD, "Threshold",
                               juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f), -20.0f,
                               [](float val, int) { return juce::String(val, 1) + " dB"; }));

    params.push_back(makeFloat(ID_RATIO, "Ratio",
                               juce::NormalisableRange<float>(1.0f, 64.0f, 0.1f, 0.35f), 4.0f,
                               [](float val, int) { return juce::String(val, 1) + ":1"; }));

    params.push_back(makeFloat(ID_ATTACK, "Attack",
                               juce::NormalisableRange<float>(0.1f, 50.0f, 0.1f, 0.35f), 1.0f,
                               [](float val, int) { return juce::String(val, 1) + " ms"; }));

    params.push_back(makeFloat(ID_HOLD, "Hold",
                               juce::NormalisableRange<float>(0.0f, 100.0f, 0.5f, 0.5f), 5.0f,
                               [](float val, int) { return juce::String(val, 1) + " ms"; }));

    params.push_back(makeFloat(ID_RELEASE, "Release",
                               juce::NormalisableRange<float>(20.0f, 2000.0f, 1.0f, 0.35f), 50.0f,
                               [](float val, int) { return juce::String(val, 0) + " ms"; }));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ID_SOFT_KNEE, 1 },
        "Soft Knee",
        true));

    params.push_back(makeFloat(ID_KNEE_WIDTH, "Knee Width",
                               juce::NormalisableRange<float>(0.0f, 40.0f, 0.5f), 10.0f,
                               [](float val, int) { return juce::String(val, 1) + " dB"; }));

    params.push_back(makeFloat(ID_MAKEUP, "Make-up Gain",
                               juce::NormalisableRange<float>(0.0f, 48.0f, 0.1f), 0.0f,
                               [](float val, int) { return juce::String(val, 1) + " dB"; }));

    // De-Esser Parameters
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ID_DEESSER_ENABLE, 1 },
        "De-Esser Enable",
        false));

    params.push_back(makeFloat(ID_DEESSER_FREQ, "De-Esser Freq",
                               juce::NormalisableRange<float>(1000.0f, 10000.0f, 10.0f, 0.4f), 4000.0f,
                               [](float val, int) { return (val >= 1000.0f) ? juce::String(val * 0.001f, 2) + " kHz" : juce::String(val, 0) + " Hz"; }));

    params.push_back(makeFloat(ID_DEESSER_THRESH, "De-Esser Rel Thresh",
                               juce::NormalisableRange<float>(-30.0f, 0.0f, 0.1f), -10.0f,
                               [](float val, int) { return juce::String(val, 1) + " dB"; }));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ID_DEESSER_MONITOR, 1 },
        "De-Esser Monitor",
        false));

    // Advanced Parameters
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ID_SOFTSAT, 1 },
        "SoftSat Crossover",
        false));

    params.push_back(makeFloat(ID_LR_IN_DIFF, "L/R In Diff",
                               juce::NormalisableRange<float>(-6.0f, 6.0f, 0.1f), 0.0f,
                               [](float val, int) { return juce::String(val, 1) + " dB"; }));

    params.push_back(makeFloat(ID_LR_OUT_DIFF, "L/R Out Diff",
                               juce::NormalisableRange<float>(-6.0f, 6.0f, 0.1f), 0.0f,
                               [](float val, int) { return juce::String(val, 1) + " dB"; }));

    params.push_back(makeFloat(ID_VCA_SMOOTH, "VCA Smooth Time",
                               juce::NormalisableRange<float>(0.1f, 10.0f, 0.1f, 0.4f), 1.0f,
                               [](float val, int) { return juce::String(val, 1) + " ms"; }));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID{ ID_SUBSAMPLE, 1 },
        "Subsampling Factor",
        juce::StringArray{ "1x (Full Rate)", "2x", "4x", "8x" },
        0));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID{ ID_BYPASS, 1 },
        "Bypass",
        false));

    return { params.begin(), params.end() };
}

bool ModelWAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainIn = layouts.getMainInputChannelSet();
    const auto& mainOut = layouts.getMainOutputChannelSet();

    if (mainIn != mainOut)
        return false;

    return (mainIn == juce::AudioChannelSet::stereo() || mainIn == juce::AudioChannelSet::mono());
}

void ModelWAudioProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    m_dspEngine.setSampleRate(sampleRate);
    m_dspEngine.reset();
}

void ModelWAudioProcessor::releaseResources()
{
}

void ModelWAudioProcessor::updateEngineParameters()
{
    m_dspEngine.setThreshold(m_apvts.getRawParameterValue(ID_THRESHOLD)->load());
    m_dspEngine.setRatio(m_apvts.getRawParameterValue(ID_RATIO)->load());
    m_dspEngine.setAttack(m_apvts.getRawParameterValue(ID_ATTACK)->load());
    m_dspEngine.setHold(m_apvts.getRawParameterValue(ID_HOLD)->load());
    m_dspEngine.setRelease(m_apvts.getRawParameterValue(ID_RELEASE)->load());
    m_dspEngine.setSoftKnee(m_apvts.getRawParameterValue(ID_SOFT_KNEE)->load() > 0.5f);
    m_dspEngine.setSoftKneeWidth(m_apvts.getRawParameterValue(ID_KNEE_WIDTH)->load());
    m_dspEngine.setMakeUpGain(m_apvts.getRawParameterValue(ID_MAKEUP)->load());

    m_dspEngine.setDeEsserEnabled(m_apvts.getRawParameterValue(ID_DEESSER_ENABLE)->load() > 0.5f);
    m_dspEngine.setDeEsserFrequency(m_apvts.getRawParameterValue(ID_DEESSER_FREQ)->load());
    m_dspEngine.setDeEsserThreshold(m_apvts.getRawParameterValue(ID_DEESSER_THRESH)->load());
    m_dspEngine.setDeEsserMonitor(m_apvts.getRawParameterValue(ID_DEESSER_MONITOR)->load() > 0.5f);

    m_dspEngine.setSoftSat(m_apvts.getRawParameterValue(ID_SOFTSAT)->load() > 0.5f);
    m_dspEngine.setLRInputDiff(m_apvts.getRawParameterValue(ID_LR_IN_DIFF)->load());
    m_dspEngine.setLROutputDiff(m_apvts.getRawParameterValue(ID_LR_OUT_DIFF)->load());
    m_dspEngine.setVCASmoothTime(m_apvts.getRawParameterValue(ID_VCA_SMOOTH)->load());

    int factorIdx = static_cast<int>(m_apvts.getRawParameterValue(ID_SUBSAMPLE)->load());
    int factor = 1 << factorIdx; // 0->1, 1->2, 2->4, 3->8
    m_dspEngine.setSubsampleRate(factor);
}

void ModelWAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (numSamples == 0 || totalNumInputChannels == 0)
        return;

    bool isBypassed = m_apvts.getRawParameterValue(ID_BYPASS)->load() > 0.5f;

    // Peak measurement helpers
    float peakInL = 1e-5f;
    float peakInR = 1e-5f;
    float peakOutL = 1e-5f;
    float peakOutR = 1e-5f;

    float* channelL = buffer.getWritePointer(0);
    float* channelR = (totalNumInputChannels > 1) ? buffer.getWritePointer(1) : channelL;

    for (int i = 0; i < numSamples; ++i) {
        float inL = std::abs(channelL[i]);
        float inR = std::abs(channelR[i]);
        if (inL > peakInL) peakInL = inL;
        if (inR > peakInR) peakInR = inR;
    }

    if (!isBypassed) {
        updateEngineParameters();

        for (int i = 0; i < numSamples; ++i) {
            m_dspEngine.process(channelL[i], channelR[i]);
        }

        m_meterGR.store(m_dspEngine.getGainReductionDB(), std::memory_order_relaxed);
    } else {
        m_meterGR.store(0.0f, std::memory_order_relaxed);
    }

    for (int i = 0; i < numSamples; ++i) {
        float outL = std::abs(channelL[i]);
        float outR = std::abs(channelR[i]);
        if (outL > peakOutL) peakOutL = outL;
        if (outR > peakOutR) peakOutR = outR;
    }

    // Convert peak values to dB
    auto toDB = [](float linear) {
        return (linear > 1e-5f) ? 20.0f * std::log10(linear) : -100.0f;
    };

    float inL_dB = toDB(peakInL);
    float inR_dB = toDB(peakInR);
    float outL_dB = toDB(peakOutL);
    float outR_dB = toDB(peakOutR);

    m_meterInL.store(inL_dB, std::memory_order_relaxed);
    m_meterInR.store(inR_dB, std::memory_order_relaxed);
    m_meterOutL.store(outL_dB, std::memory_order_relaxed);
    m_meterOutR.store(outR_dB, std::memory_order_relaxed);

    m_currentInputDB.store(std::max(inL_dB, inR_dB), std::memory_order_relaxed);
    m_currentOutputDB.store(std::max(outL_dB, outR_dB), std::memory_order_relaxed);
}

juce::AudioProcessorEditor* ModelWAudioProcessor::createEditor()
{
    return new ModelWAudioProcessorEditor(*this);
}

void ModelWAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = m_apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ModelWAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(m_apvts.state.getType())) {
        m_apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

// Juce entry point
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ModelWAudioProcessor();
}
