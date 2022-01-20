#pragma once

#include "DryWetProcessor.h"

class ChainIOProcessor
{
public:
    explicit ChainIOProcessor (AudioProcessorValueTreeState& vts, std::function<void (int)>&& latencyChangedCallback);

    static void createParameters (Parameters& params);
    void prepare (double sampleRate, int samplesPerBlock);

    int getOversamplingFactor() const;
    dsp::AudioBlock<float> processAudioInput (AudioBuffer<float>& buffer, bool& sampleRateChanged);
    void processAudioOutput (AudioBuffer<float>& buffer);

    auto& getOversampling() { return oversampling; }

private:
    const std::function<void (int)> latencyChangedCallbackFunc;

    chowdsp::VariableOversampling<float> oversampling;

    std::atomic<float>* inGainParam = nullptr;
    std::atomic<float>* outGainParam = nullptr;
    dsp::Gain<float> inGain, outGain;

    std::atomic<float>* dryWetParam = nullptr;
    DryWetProcessor dryWetMixer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ChainIOProcessor)
};
