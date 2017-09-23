// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/layers/activation_layer.h"

namespace fdeep { namespace internal
{

// https://arxiv.org/pdf/1706.02515.pdf
class selu_layer : public activation_layer
{
public:
    explicit selu_layer(const std::string& name)
        : activation_layer(name)
    {
    }
protected:
    const float_t alpha_ = 1.6732632423543772848170429916717;
    const float_t scale_ = 1.0507009873554804934193349852946;
    tensor3 transform_input(const tensor3& in_vol) const override
    {
        auto activation_function = [this](float_t x) -> float_t
        {
            return scale_ * (x >= 0.0 ? x : alpha_ * std::exp(x) - alpha_);
        };
        return transform_tensor3(activation_function, in_vol);
    }
};

} } // namespace fdeep, namespace internal
