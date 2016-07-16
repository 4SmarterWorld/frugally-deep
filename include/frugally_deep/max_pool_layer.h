// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/pool_layer.h"

namespace fd
{

class max_pool_layer : public pool_layer
{
public:
    explicit max_pool_layer(std::size_t scale_factor) :
            pool_layer(scale_factor)
    {
    }
private:
    float_t pool_helper_max(float_t& acc, float_t v)
    {
        return std::max(acc, v);
    }
    matrix3d pool(const matrix3d& in_vol) const override
    {
        float_t pool_helper_acc_init = std::numeric_limits<float>::min();
        auto pool_helper_max = [](float_t& acc, float_t val) -> void
        {
            acc = std::max(acc, val);
        };
        auto pool_helper_identity = [this](float_t acc) -> float_t
        {
            return acc / scale_factor_ * scale_factor_;
        };
        return pool_helper(
            scale_factor_,
            pool_helper_acc_init,
            pool_helper_max,
            pool_helper_identity,
            in_vol);
    }
};

} // namespace fd
