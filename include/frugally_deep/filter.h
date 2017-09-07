// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/typedefs.h"

#include "frugally_deep/matrix3d.h"
#include "frugally_deep/size3d.h"

#include <cassert>
#include <cstddef>
#include <vector>

namespace fd
{

class filter
{
public:
    filter(const matrix3d& m, float_t bias) : m_(m), bias_(bias)
    {
    }
    const size3d& size() const
    {
        return m_.size();
    }
    const matrix3d& get_matrix3d() const
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
        assert(weights.size() == m_.size().volume());
        m_ = matrix3d(m_.size(), weights);
        bias_ = bias;
    }
private:
    matrix3d m_;
    float_t bias_;
};

typedef std::vector<filter> filter_vec;

inline filter_vec flip_filters_spatially(const filter_vec& fs)
{
    assert(!fs.empty());
    std::size_t k = fs.size();
    std::size_t d = fs.front().size().depth_;
    size3d new_filter_size(
        k,
        fs.front().size().height_,
        fs.front().size().width_);
    filter_vec result;
    result.reserve(d);
    for (std::size_t i = 0; i < d; ++i)
    {
        matrix3d new_f_mat(new_filter_size);
        float_t bias = 0;
        for (std::size_t j = 0; j < k; ++j)
        {
            for (std::size_t y = 0; y < new_filter_size.height_; ++y)
            {
                //std::size_t y2 = new_filter_size.height_ - (y + 1);
                for (std::size_t x = 0; x < new_filter_size.width_; ++x)
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

} // namespace fd
