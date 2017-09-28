// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/common.hpp"

#include "frugally_deep/shape2.hpp"

#include <cstddef>
#include <cstdlib>
#include <string>

namespace fdeep { namespace internal
{

class shape3
{
public:
    explicit shape3(
        std::size_t depth,
        std::size_t height,
        std::size_t width) :
            depth_(depth),
            height_(height),
            width_(width)
    {
    }
    std::size_t volume() const
    {
        return depth_ * height_ * width_;
    }

    shape2 without_depth() const
    {
        return shape2(height_, width_);
    }

    std::size_t depth_;
    std::size_t height_;
    std::size_t width_;
};

inline bool operator == (const shape3& lhs, const shape3& rhs)
{
    return
        lhs.depth_ == rhs.depth_ &&
        lhs.height_ == rhs.height_ &&
        lhs.width_ == rhs.width_;
}

inline bool operator != (const shape3& lhs, const shape3& rhs)
{
    return !(lhs == rhs);
}

inline std::string show_shape3(const shape3& size)
{
    return std::string(
        "(" + std::to_string(size.depth_) +
        "," + std::to_string(size.height_) +
        "," + std::to_string(size.width_) +
        ")");
}

} // namespace internal

using shape3 = internal::shape3;

} // namespace fdeep
