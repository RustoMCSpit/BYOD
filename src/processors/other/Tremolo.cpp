#include "Tremolo.h"

Tremolo::Tremolo (UndoManager* um) : BaseProcessor ("Tremolo", createParameterLayout(), um)
{
    rateParam = vts.getRawParameterValue ("rate");
    waveParam = vts.getRawParameterValue ("wave");
    depthParam = vts.getRawParameterValue ("depth");

    uiOptions.backgroundColour = Colours::orange.darker (0.1f);
    uiOptions.powerColour = Colours::cyan;
    uiOptions.info.description = "A simple tremolo effect.";
    uiOptions.info.authors = StringArray { "Jatin Chowdhury" };
}

AudioProcessorValueTreeState::ParameterLayout Tremolo::createParameterLayout()
{
    using namespace ParameterHelpers;
    auto params = createBaseParams();
    createFreqParameter (params, "rate", "Rate", 1.0f, 10.0f, 2.0f, 2.0f);
    createPercentParameter (params, "wave", "Wave", 0.5f);
    createPercentParameter (params, "depth", "Depth", 0.5f);
    
    return { params.begin(), params.end() };
}

void Tremolo::prepare (double sampleRate, int samplesPerBlock)
{
    dsp::ProcessSpec monoSpec { sampleRate, (uint32) samplesPerBlock, 1 };
    sine.prepare (monoSpec);
    
    filter.prepare (monoSpec);
    filter.setCutoffFrequency (1000.0f);

    waveBuffer.setSize (1, samplesPerBlock);
    phaseSmooth.reset (sampleRate, 0.01);
    waveSmooth.reset (sampleRate, 0.01);
    
    fs = (float) sampleRate;
    phase = 0.0f;
}

void Tremolo::fillWaveBuffer (float* waveBuffer, const int numSamples, float& phase)
{
    bool isSmoothing = phaseSmooth.isSmoothing() || waveSmooth.isSmoothing();

    if (isSmoothing)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            auto curWave = waveSmooth.getNextValue();
            auto sineGain = 1.0f - jmin (2.0f * curWave, 1.0f);
            auto squareGain = jmax (2.0f * (curWave - 0.5f), 0.0f);
            auto triGain = 1.0f - 2.0f * std::abs (0.5f - curWave);

            waveBuffer[n] = sineGain * dsp::FastMathApproximations::sin (phase); // sine
            waveBuffer[n] += triGain * phase / MathConstants<float>::pi; // triangle
            waveBuffer[n] += squareGain * (phase > 0.0f ? 1.0f : -1.0f); // square

            phase += phaseSmooth.getNextValue();
            phase = phase > MathConstants<float>::pi ? phase - MathConstants<float>::twoPi : phase;
        }
    }
    else
    {
        auto phaseInc = phaseSmooth.getNextValue();
        auto curWave = waveSmooth.getNextValue();
        if (curWave <= 0.5f)
        {
            auto sineGain = 1.0f - jmin (2.0f * curWave, 1.0f);
            auto triGain = 1.0f - 2.0f * std::abs (0.5f - curWave);
            for (int n = 0; n < numSamples; ++n)
            {
                waveBuffer[n] = sineGain * dsp::FastMathApproximations::sin (phase); // sine
                waveBuffer[n] += triGain * phase / MathConstants<float>::pi; // triangle

                phase += phaseInc;
                phase = phase > MathConstants<float>::pi ? phase - MathConstants<float>::twoPi : phase;
            }
        }
        else
        {
            auto squareGain = jmax (2.0f * (curWave - 0.5f), 0.0f);
            auto triGain = 1.0f - 2.0f * std::abs (0.5f - curWave);
            for (int n = 0; n < numSamples; ++n)
            {
                waveBuffer[n] = triGain * phase / MathConstants<float>::pi; // triangle
                waveBuffer[n] += squareGain * (phase > 0.0f ? 1.0f : -1.0f); // square

                phase += phaseInc;
                phase = phase > MathConstants<float>::pi ? phase - MathConstants<float>::twoPi : phase;
            }
        }
    }
}

void Tremolo::processAudio (AudioBuffer<float>& buffer)
{
    const auto numSamples = buffer.getNumSamples();
    waveBuffer.setSize (1, numSamples, false, false, true);
    phaseSmooth.setTargetValue (*rateParam * MathConstants<float>::pi / fs);
    waveSmooth.setTargetValue (*waveParam);
    
    // fill wave buffer (-1, 1)
    dsp::AudioBlock<float> waveBlock { waveBuffer };
    dsp::ProcessContextReplacing<float> waveCtx { waveBlock };
    fillWaveBuffer (waveBlock.getChannelPointer (0), numSamples, phase);

    auto depthVal = depthParam->load(); // skew this?
    waveBlock *= 0.5f * depthVal;
    waveBlock += 1.0f - depthVal;
    filter.process<decltype (waveCtx), chowdsp::StateVariableFilterType::Lowpass> (waveCtx);

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        FloatVectorOperations::multiply (x, x, waveBuffer.getReadPointer (0), numSamples);
    }
}