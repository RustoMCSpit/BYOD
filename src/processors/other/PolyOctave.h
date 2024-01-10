#pragma once

#include "processors/BaseProcessor.h"

class PolyOctave : public BaseProcessor
{
public:
    explicit PolyOctave (UndoManager* um);

    ProcessorType getProcessorType() const override { return Other; }
    static ParamLayout createParameterLayout();

    void prepare (double sampleRate, int samplesPerBlock) override;
    void processAudio (AudioBuffer<float>& buffer) override;

    struct ERBFilterBank
    {
        static constexpr size_t numFilterBands = 44;
        using float_2 = xsimd::batch<double>;
        std::array<chowdsp::IIRFilter<4, float_2>, numFilterBands / float_2::size> erbFilterReal, erbFilterImag;
    };

private:
    chowdsp::SmoothedBufferValue<double> dryGain {};
    chowdsp::SmoothedBufferValue<double> upOctaveGain {};
    chowdsp::SmoothedBufferValue<double> up2OctaveGain {};

    chowdsp::Buffer<double> doubleBuffer;
    chowdsp::Buffer<double> upOctaveBuffer;
    chowdsp::Buffer<double> up2OctaveBuffer;

    std::array<ERBFilterBank, 2> octaveUpFilterBank;
    std::array<ERBFilterBank, 2> octaveUp2FilterBank;

    chowdsp::FirstOrderHPF<float> dcBlocker;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PolyOctave)
};
