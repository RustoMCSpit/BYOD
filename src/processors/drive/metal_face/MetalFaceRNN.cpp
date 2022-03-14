#include "MetalFaceRNN.h"

namespace
{
using Vec2d = std::vector<std::vector<float>>;
auto transpose = [] (const Vec2d& x) -> Vec2d
{
    auto outer_size = x.size();
    auto inner_size = x[0].size();
    Vec2d y (inner_size, std::vector<float> (outer_size, 0.0f));

    for (size_t i = 0; i < outer_size; ++i)
    {
        for (size_t j = 0; j < inner_size; ++j)
            y[j][i] = x[i][j];
    }

    return std::move (y);
};

template <typename ModelType>
void loadModel (ModelType& model, int hiddenSize, const nlohmann::json& weights_json)
{
    auto& lstm = model.template get<0>();
    auto& dense = model.template get<1>();

    Vec2d lstm_weights_ih = weights_json["/state_dict/rec.weight_ih_l0"_json_pointer];
    lstm.setWVals (transpose (lstm_weights_ih));

    Vec2d lstm_weights_hh = weights_json["/state_dict/rec.weight_hh_l0"_json_pointer];
    lstm.setUVals (transpose (lstm_weights_hh));

    std::vector<float> lstm_bias_ih = weights_json["/state_dict/rec.bias_ih_l0"_json_pointer];
    std::vector<float> lstm_bias_hh = weights_json["/state_dict/rec.bias_hh_l0"_json_pointer];
    for (int i = 0; i < 4 * hiddenSize; ++i)
        lstm_bias_hh[(size_t) i] += lstm_bias_ih[(size_t) i];
    lstm.setBVals (lstm_bias_hh);

    Vec2d dense_weights = weights_json["/state_dict/lin.weight"_json_pointer];
    dense.setWeights (dense_weights);

    std::vector<float> dense_bias = weights_json["/state_dict/lin.bias"_json_pointer];
    dense.setBias (dense_bias.data());
}
} // namespace

template <int hiddenSize, typename ResamplerType>
void MetalFaceRNN<hiddenSize, ResamplerType>::initialise (const void* modelData, int modelDataSize, double modelSampleRate)
{
    targetSampleRate = modelSampleRate;

    MemoryInputStream jsonInputStream (modelData, (size_t) modelDataSize, false);
    auto weightsJson = nlohmann::json::parse (jsonInputStream.readEntireStreamAsString().toStdString());

    loadModel (model, hiddenSize, weightsJson);
}

template <int hiddenSize, typename ResamplerType>
void MetalFaceRNN<hiddenSize, ResamplerType>::prepare (double sampleRate, int samplesPerBlock)
{
    auto targetSampleRateToUse = targetSampleRate;
    if (sampleRate > targetSampleRateToUse)
    {
        auto factor = std::log2 ((sampleRate / targetSampleRateToUse));
        if (factor != (int) factor)
            factor = (float) (int) factor + 1.0f;

        targetSampleRateToUse = sampleRate / std::pow (2.0f, factor);
    }

    needsResampling = sampleRate != targetSampleRateToUse;
    resampler.prepareWithTargetSampleRate ({ sampleRate, (uint32) samplesPerBlock, 1 }, targetSampleRateToUse);

    model.reset();
}

template <int hiddenSize, typename ResamplerType>
void MetalFaceRNN<hiddenSize, ResamplerType>::reset()
{
    resampler.reset();
    model.reset();
}

template <int hiddenSize, typename ResamplerType>
void MetalFaceRNN<hiddenSize, ResamplerType>::process (dsp::AudioBlock<float>& block)
{
    auto&& processBlock = dsp::AudioBlock<float> { block };
    if (needsResampling)
        processBlock = resampler.processIn (block);

    const auto numSamples = (int) processBlock.getNumSamples();
    auto* x = processBlock.getChannelPointer (0);
    for (int i = 0; i < numSamples; ++i)
        x[i] = model.forward (&x[i]);

    if (needsResampling)
        resampler.processOut (processBlock, block);
}

//=======================================================
template class MetalFaceRNN<28, chowdsp::ResamplingTypes::LanczosResampler<>>;