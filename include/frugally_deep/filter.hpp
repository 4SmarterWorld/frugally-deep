// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/common.hpp"

#include "frugally_deep/tensor3.hpp"
#include "frugally_deep/shape3.hpp"

#include <cassert>
#include <cstddef>
#include <vector>

namespace fdeep { namespace internal
{

class filter
{
public:
    filter(const tensor3& m, float_t bias) : m_(m), bias_(bias)
    {
    }
    const shape3& shape() const
    {
        return m_.shape();
    }
    std::size_t volume() const
    {
        return m_.shape().volume();
    }
    const tensor3& get_tensor3() const
    {
        return m_;
    }
    float_t get(std::size_t z, std::size_t y, size_t x) const
    {
        return m_.get(z, y, x);
    }
    float_t get_bias() const
    {
        return bias_;
    }
    void set_params(const float_vec& weights, float_t bias)
    {
        assertion(weights.size() == m_.shape().volume(),
            "invalid parameter count");
        m_ = tensor3(m_.shape(), weights);
        bias_ = bias;
    }
private:
    tensor3 m_;
    float_t bias_;
};

typedef std::vector<filter> filter_vec;

inline filter_vec generate_filters(
    const shape3& filter_size, std::size_t k,
    const float_vec& weights, const float_vec& bias)
{
    filter_vec filters(k, filter(tensor3(
        shape3(filter_size)), 0));

    assertion(!filters.empty(), "at least one filter needed");
    const std::size_t param_count = fplus::sum(fplus::transform(
            std::mem_fn(&filter::volume), filters));

    assertion(static_cast<std::size_t>(weights.size()) == param_count,
        "invalid weight size");
    const auto filter_param_cnt = filters.front().shape().volume();

    const auto filter_weights =
        fplus::split_every(filter_param_cnt, weights);
    assertion(filter_weights.size() == filters.size(),
        "invalid size of filter weights");
    assertion(bias.size() == filters.size(), "invalid bias size");
    auto it_filter_val = std::begin(filter_weights);
    auto it_filter_bias = std::begin(bias);
    for (auto& filt : filters)
    {
        filt.set_params(*it_filter_val, *it_filter_bias);
        ++it_filter_val;
        ++it_filter_bias;
    }
    return filters;
}

inline filter_vec flip_filters_spatially(const filter_vec& fs)
{
    assertion(!fs.empty(), "no filter given");
    std::size_t k = fs.size();
    std::size_t d = fs.front().shape().depth_;
    shape3 new_filter_shape(
        k,
        fs.front().shape().height_,
        fs.front().shape().width_);
    filter_vec result;
    result.reserve(d);
    for (std::size_t i = 0; i < d; ++i)
    {
        tensor3 new_f_mat(new_filter_shape);
        float_t bias = 0;
        for (std::size_t j = 0; j < k; ++j)
        {
            for (std::size_t y = 0; y < new_filter_shape.height_; ++y)
            {
                //std::size_t y2 = new_filter_size.height_ - (y + 1);
                for (std::size_t x = 0; x < new_filter_shape.width_; ++x)
                {
                    //std::size_t x2 = new_filter_size.width_ - (x + 1);
                    new_f_mat.set(j, y, x, fs[j].get(i, y, x));
                }
            }
            bias += fs[j].get_bias() / static_cast<float_t>(k);
        }
        result.push_back(filter(new_f_mat, bias));
    }
    return result;
}

} } // namespace fdeep, namespace internal
