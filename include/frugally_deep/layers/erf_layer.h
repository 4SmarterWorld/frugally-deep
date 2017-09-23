// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/layers/activation_layer.h"

namespace fd
{

class erf_layer : public activation_layer
{
public:
    erf_layer(const std::string& name)
        : activation_layer(name)
    {
    }
protected:
    tensor3 transform_input(const tensor3& in_vol) const override
    {
        auto activation_function = [](float_t x) -> float_t
        {
            return std::erf(x);
        };
        return transform_tensor3(activation_function, in_vol);
    }
};

} // namespace fd
