#include "BigMuffDrive.h"
#include "../../ParameterHelpers.h"

namespace
{
const auto cutoffRange = ParameterHelpers::createNormalisableRange (500.0f, 22000.0f, 1200.0f);
float harmParamToCutoffHz (float harmParam)
{
    return cutoffRange.convertFrom0to1 (harmParam);
}

const auto sustainRange = ParameterHelpers::createNormalisableRange (0.4f, 2.0f, 1.0f);
const auto levelRange = ParameterHelpers::createNormalisableRange (-60.0f, 0.0f, -9.0f);
} // namespace

BigMuffDrive::BigMuffDrive (UndoManager* um) : BaseProcessor ("Big Muff Drive", createParameterLayout(), um)
{
    sustainParam = vts.getRawParameterValue ("sustain");
    harmParam = vts.getRawParameterValue ("harmonics");
    levelParam = vts.getRawParameterValue ("level");
    nStagesParam = vts.getRawParameterValue ("n_stages");

    uiOptions.backgroundColour = Colours::darkgrey.brighter (0.3f).withRotatedHue (0.2f);
    uiOptions.powerColour = Colours::red.brighter (0.15f);
    uiOptions.info.description = "Fuzz effect based on the drive stage from the Electro-Harmonix Big Muff Pi.";
    uiOptions.info.authors = StringArray { "Jatin Chowdhury" };
}

AudioProcessorValueTreeState::ParameterLayout BigMuffDrive::createParameterLayout()
{
    using namespace ParameterHelpers;

    auto params = createBaseParams();

    createPercentParameter (params, "sustain", "Sustain", 0.5f);
    createPercentParameter (params, "harmonics", "Harmonics", 0.65f);
    createPercentParameter (params, "level", "Level", 0.65f);

    emplace_param<AudioParameterChoice> (params, "n_stages", "Stages", StringArray { "1 Stage", "2 Stages", "3 Stages", "4 Stages" }, 1);

    return { params.begin(), params.end() };
}

void BigMuffDrive::prepare (double sampleRate, int samplesPerBlock)
{
    fs = (float) sampleRate;

    cutoffSmooth.reset (sampleRate, 0.02);
    cutoffSmooth.setCurrentAndTargetValue (harmParamToCutoffHz (*harmParam));
    for (auto& filt : inputFilter)
    {
        filt.calcCoefs (cutoffSmooth.getTargetValue(), fs);
        filt.reset();
    }

    for (auto& stage : stages)
        stage.prepare (sampleRate);

    auto spec = dsp::ProcessSpec { sampleRate, (uint32) samplesPerBlock, 2 };

    sustainGain.prepare (spec);
    sustainGain.setRampDurationSeconds (0.02);

    outLevel.prepare (spec);
    outLevel.setRampDurationSeconds (0.02);

    for (auto& filt : dcBlocker)
    {
        filt.calcCoefs (16.0f, fs);
        filt.reset();
    }

    // pre-buffering
    AudioBuffer<float> buffer (2, samplesPerBlock);
    for (int i = 0; i < 10000; i += samplesPerBlock)
    {
        buffer.clear();
        processAudio (buffer);
    }
}

void BigMuffDrive::processInputStage (AudioBuffer<float>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    cutoffSmooth.setTargetValue (harmParamToCutoffHz (*harmParam));
    if (cutoffSmooth.isSmoothing())
    {
        if (numChannels == 1)
        {
            auto* x = buffer.getWritePointer (0);
            for (int n = 0; n < numSamples; ++n)
            {
                inputFilter[0].calcCoefs (cutoffSmooth.getNextValue(), fs);
                x[n] = inputFilter[0].processSample (x[n]);
            }
        }
        else if (numChannels == 2)
        {
            auto* xL = buffer.getWritePointer (0);
            auto* xR = buffer.getWritePointer (1);
            for (int n = 0; n < numSamples; ++n)
            {
                auto cutoffHz = cutoffSmooth.getNextValue();

                inputFilter[0].calcCoefs (cutoffHz, fs);
                xL[n] = inputFilter[0].processSample (xL[n]);

                inputFilter[1].calcCoefs (cutoffHz, fs);
                xR[n] = inputFilter[1].processSample (xR[n]);
            }
        }
    }
    else
    {
        for (int ch = 0; ch < numChannels; ++ch)
        {
            inputFilter[ch].calcCoefs (cutoffSmooth.getNextValue(), fs);
            inputFilter[ch].processBlock (buffer.getWritePointer (ch), numSamples);
        }
    }

    sustainGain.setGainLinear (sustainRange.convertFrom0to1 (*sustainParam));
    dsp::AudioBlock<float> block { buffer };
    sustainGain.process (dsp::ProcessContextReplacing<float> { block });
}

void BigMuffDrive::processAudio (AudioBuffer<float>& buffer)
{
    const auto numChannels = buffer.getNumChannels();
    const auto numSamples = buffer.getNumSamples();

    processInputStage (buffer);

    const int numStages = (int) *nStagesParam + 1;
    for (int i = 0; i < numStages; ++i)
        stages[i].processBlock (buffer);

    for (int ch = 0; ch < numChannels; ++ch)
        dcBlocker[ch].processBlock (buffer.getWritePointer (ch), numSamples);

    auto outGain = Decibels::decibelsToGain (levelRange.convertFrom0to1 (*levelParam), levelRange.start);
    outGain *= Decibels::decibelsToGain (13.0f); // makeup from level lost in clipping stages
    outGain *= numStages % 2 == 0 ? 1.0f : -1.0f;
    outLevel.setGainLinear (outGain);
    dsp::AudioBlock<float> block { buffer };
    outLevel.process (dsp::ProcessContextReplacing<float> { block });
}