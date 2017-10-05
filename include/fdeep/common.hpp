// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include <cmath>
#include <vector>
#include <string>
#include <stdexcept>

namespace fdeep { namespace internal
{

typedef float float_t;
typedef std::vector<float_t> float_vec;
typedef float_vec::const_iterator float_vec_const_it;
const float_t pi = static_cast<float_t>(std::acos(-1));

inline std::runtime_error error(const std::string& error)
{
    return std::runtime_error(error);
}

inline void raise_error(const std::string& msg)
{
    throw error(msg);
}

inline void assertion(bool cond, const std::string& error)
{
    if (!cond)
    {
        raise_error(error);
    }
}

} } // namespace fdeep, namespace internal
