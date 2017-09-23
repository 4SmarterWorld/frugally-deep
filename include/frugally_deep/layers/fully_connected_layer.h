// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/layers/layer.h"

#include "frugally_deep/tensor2.h"

#include <fplus/fplus.hpp>

namespace fd
{

// Takes a single stack volume (shape3(n, 1, 1)) as input.
class fully_connected_layer : public layer
{
public:
    static tensor2 generate_params(std::size_t n_in,
        const float_vec& weights, const float_vec& bias)
    {
        return tensor2(shape2(n_in + 1, bias.size()),
            fplus::append(weights, bias));
    }
    fully_connected_layer(const std::string& name, std::size_t units,
            const float_vec& weights,
            const float_vec& bias) :
        layer(name),
        n_in_(weights.size() / bias.size()),
        n_out_(units),
        params_(generate_params(n_in_, weights, bias))
    {
        assertion(bias.size() == units, "invalid bias count");
        assertion(weights.size() % units == 0, "invalid weight count");
    }
protected:
    tensor3s apply_impl(const tensor3s& inputs) const override
    {
        assertion(inputs.size() == 1, "invalid number of input tensors");
        const auto& input = inputs[0];
        assertion(input.size().height_ == 1, "input needs to be flattened");
        assertion(input.size().depth_ == 1, "input needs to be flattened");
        const auto bias_padded_input = bias_pad_input(input);
        return {tensor3(shape3(1, 1, n_out_),
            multiply(bias_padded_input, params_).as_vector())};
    }
    static tensor2 bias_pad_input(const tensor3& input)
    {
        return tensor2(
            shape2(1, input.size().width_ + 1),
            fplus::append(input.as_vector(), {1}));
    }
    std::size_t n_in_;
    std::size_t n_out_;
    tensor2 params_;
};

} // namespace fd
