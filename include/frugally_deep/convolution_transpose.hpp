// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/typedefs.hpp"

#include "frugally_deep/filter.hpp"
#include "frugally_deep/convolution.hpp"

#include <cassert>
#include <cstddef>
#include <vector>

namespace fdeep { namespace internal
{

namespace details
{

inline tensor2 unpad_tensor2(
    std::size_t padding_y,
    std::size_t padding_x,
    const tensor2& in)
{
    tensor2 out(shape2(
        in.size().height_ - 2 * padding_y,
        in.size().width_ - 2 * padding_x));
    for (std::size_t y = 0; y < out.size().height_; ++y)
    {
        for (std::size_t x = 0; x < out.size().width_; ++x)
        {
            out.set(y, x, in.get(y + padding_y, x + padding_x));
        }
    }
    return out;
}

inline void convolve_transpose_go(
    std::size_t stride,
    const tensor2& filter,
    const tensor2& in,
    tensor2& out)
{
    const std::size_t fy = filter.size().height_;
    const std::size_t fx = filter.size().width_;
    for (std::size_t y = 0; y < in.size().height_; ++y)
    {
        for (std::size_t x = 0; x < in.size().width_; ++x)
        {
            for (std::size_t yf = 0; yf < fy; ++yf)
            {
                for (std::size_t xf = 0; xf < fx; ++xf)
                {
                    const float_t add_val = filter.get(yf, xf) *
                        in.get(y, x);
                    out.set(stride * y + yf, stride * x + xf,
                        out.get(stride * y + yf, stride * x + xf)
                        + add_val);
                }
            }
        }
    }
}


template <
    std::size_t stride,
    std::size_t fy,
    std::size_t fx
    >
void convolve_transpose_go_template(
    const tensor2& filter,
    const tensor2& in,
    tensor2& out)
{
    assert(filter.size().height_ == fy);
    assert(filter.size().width_ == fx);
    for (std::size_t y = 0; y < in.size().height_; ++y)
    {
        for (std::size_t x = 0; x < in.size().width_; ++x)
        {
            for (std::size_t yf = 0; yf < fy; ++yf)
            {
                for (std::size_t xf = 0; xf < fx; ++xf)
                {
                    const float_t add_val = filter.get(yf, xf) *
                        in.get(y, x);
                    out.set(stride * y + yf, stride * x + xf,
                        out.get(stride * y + yf, stride * x + xf)
                        + add_val);
                }
            }
        }
    }
}

} // namespace details

// todo: noch ist alles falsch
/*

0xxxx0
fff
 fff
  fff
   fff
 yyyy


0xxxx0
ffff
  ffff
 yy
 */
inline tensor2 convolve_transpose(
    std::size_t stride,
    std::size_t unpadding_y,
    std::size_t unpadding_x,
    const tensor2& filter,
    const tensor2& in)
{
    assert(stride > 0);
    const std::size_t h1 = in.size().height_;
    const std::size_t w1 = in.size().width_;
    const std::size_t fy = filter.size().height_;
    const std::size_t fx = filter.size().width_;
    const std::size_t h2 = fy - 2 * unpadding_y + stride * (h1 - 1);
    const std::size_t w2 = fx - 2 * unpadding_x + stride * (w1 - 1);

    tensor2 out(shape2(h2 + 2 * unpadding_y, w2 + 2 * unpadding_x));

    if (stride == 1 && fy == 1 && fx == 1)
        details::convolve_transpose_go_template<1, 1, 1>(filter, in, out);

    else if (stride == 1 && fy == 3 && fx == 3)
        details::convolve_transpose_go_template<1, 3, 3>(filter, in, out);
    else if (stride == 1 && fy == 5 && fx == 5)
        details::convolve_transpose_go_template<1, 5, 5>(filter, in, out);

    else if (stride == 2 && fy == 1 && fx == 1)
        details::convolve_transpose_go_template<2, 1, 1>(filter, in, out);

    else if (stride == 2 && fy == 3 && fx == 3)
        details::convolve_transpose_go_template<2, 3, 3>(filter, in, out);
    else if (stride == 2 && fy == 5 && fx == 5)
        details::convolve_transpose_go_template<2, 5, 5>(filter, in, out);

    else
        details::convolve_transpose_go(stride, filter, in, out);

    return details::unpad_tensor2(unpadding_y, unpadding_x, out);
}

inline tensor3 convolve_transpose(
    std::size_t stride,
    std::size_t unpadding_y,
    std::size_t unpadding_x,
    const tensor2& filter,
    const tensor3& in)
{
    const auto conv_transpose_func = [&](const tensor2& in_slice)
    {
        return convolve_transpose(
            stride, unpadding_y, unpadding_x, filter, in_slice);
    };
    return
        tensor3_from_depth_slices(
            fplus::transform(
                conv_transpose_func,
                tensor3_to_depth_slices(in)));
}

inline tensor3 convolve_transpose(
    std::size_t stride,
    std::size_t unpadding_y,
    std::size_t unpadding_x,
    const tensor3& filters,
    const tensor2& in)
{
    const auto conv_transpose_func = [&](const tensor2& filter_slice)
    {
        return convolve_transpose(
            stride, unpadding_y, unpadding_x, filter_slice, in);
    };
    return
        tensor3_from_depth_slices(
            fplus::transform(
                conv_transpose_func,
                tensor3_to_depth_slices(filters)));
}

inline tensor2 convolve_transpose(
    std::size_t stride,
    std::size_t unpadding_y,
    std::size_t unpadding_x,
    const tensor3& filter,
    const tensor3& in)
{
    const auto conv_transpose_func = [&](
        const tensor2& filter_slice, const tensor2& in_slice)
    {
        return convolve_transpose(
            stride, unpadding_y, unpadding_x, filter_slice, in_slice);
    };
    return
        sum_tensor2s(
            fplus::zip_with(
                conv_transpose_func,
                tensor3_to_depth_slices(filter),
                tensor3_to_depth_slices(in)));
}

inline tensor2 convolve_transpose(
    std::size_t stride,
    std::size_t unpadding_y,
    std::size_t unpadding_x,
    const filter& f,
    const tensor3& in)
{
    const auto without_bias = convolve_transpose(
        stride, unpadding_y, unpadding_x, f.get_tensor3(), in);
    const auto add_bias = [&](const float_t val)
    {
        return val + f.get_bias();
    };
    return transform_tensor2(add_bias, without_bias);
}

inline tensor3 convolve_transpose(
    std::size_t stride,
    std::size_t unpadding_y,
    std::size_t unpadding_x,
    const std::vector<filter>& filters,
    const tensor3& in)
{
    // todo: convolve_transpose_matrix_mult instead of convolve with loops?
    //     (i.e. use im_to_col and matrix multiplication for performance)

    const auto convolve_transpose_in = [&](const filter& f)
    {
        return convolve_transpose(stride, unpadding_y, unpadding_x, f, in);
    };

    return tensor3_from_depth_slices(
        fplus::transform(convolve_transpose_in, filters));
}

} } // namespace fdeep, namespace internal
