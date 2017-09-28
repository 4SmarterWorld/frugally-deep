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
    const shape3& filter_shape, std::size_t k,
    const float_vec& weights, const float_vec& bias)
{
    filter_vec filters(k, filter(tensor3(
        shape3(filter_shape)), 0));

    assertion(!filters.empty(), "at least one filter needed");
    const std::size_t param_count = fplus::sum(fplus::transform(
        fplus_c_mem_fn_t(filter, volume, std::size_t), filters));

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

} } // namespace fdeep, namespace internal
