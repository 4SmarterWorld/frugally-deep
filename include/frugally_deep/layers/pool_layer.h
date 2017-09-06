// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/layers/layer.h"

#include <fplus/fplus.hpp>

#include <cassert>
#include <cstddef>
#include <vector>

namespace fd
{

// Abstract base class for pooling layers
class pool_layer : public layer
{
public:
    explicit pool_layer(const std::string& name, std::size_t scale_factor) :
        layer(name),
        scale_factor_(scale_factor)
    {
        //assert(size_in.height_ % scale_factor == 0);
        //assert(size_in.width_ % scale_factor == 0);
    }
protected:
    matrix3ds apply(const matrix3ds& inputs) const override final
    {
        assert(inputs.size() == 1);
        const auto& input = inputs[0];
        return {pool(input)};
    }

    const std::size_t scale_factor_;
    virtual matrix3d pool(const matrix3d& input) const = 0;

    template <typename AccPixelFunc, typename AccPixelFunc2,
            typename FinalizePixelFunc>
    static matrix3d pool_helper(
            std::size_t scale_factor,
            float_t acc_init,
            float_t acc2_init,
            AccPixelFunc acc_pixel_func,
            AccPixelFunc2 acc2_pixel_func,
            FinalizePixelFunc finalize_pixel_func,
            const matrix3d& in_vol)
    {
        assert(in_vol.size().height_ % scale_factor == 0);
        assert(in_vol.size().width_ % scale_factor == 0);
        matrix3d out_vol(
            size3d(
                in_vol.size().depth_,
                in_vol.size().height_ / scale_factor,
                in_vol.size().width_ / scale_factor));
        for (std::size_t z = 0; z < in_vol.size().depth_; ++z)
        {
            for (std::size_t y = 0; y < out_vol.size().height_; ++y)
            {
                std::size_t y_in = y * scale_factor;
                for (std::size_t x = 0; x < out_vol.size().width_; ++x)
                {
                    std::size_t x_in = x * scale_factor;
                    float_t acc = acc_init;
                    float_t acc2 = acc2_init;
                    for (std::size_t yf = 0; yf < scale_factor; ++yf)
                    {
                        for (std::size_t xf = 0; xf < scale_factor; ++xf)
                        {
                            const float_t val =
                                in_vol.get(z, y_in + yf, x_in + xf);
                            acc = acc_pixel_func(acc, val);
                            acc2 = acc2_pixel_func(acc2, val);
                        }
                    }
                    out_vol.set(z, y, x, finalize_pixel_func(acc, acc2));
                }
            }
        }
        return out_vol;
    }
};

} // namespace fd
