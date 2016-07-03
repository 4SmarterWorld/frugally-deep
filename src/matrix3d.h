#pragma once

#include "size3d.h"

#include <cstddef>
#include <string>
#include <vector>

class matrix3d
{
public:
    explicit matrix3d(const size3d& size) :
        size_(size),
        values_(size.area(), 0.0f)
    {
    }
    float get(std::size_t z, std::size_t y, size_t x) const
    {
        return values_[idx(z, y, x)];
    }
    void set(std::size_t z, std::size_t y, size_t x, float value)
    {
        values_[idx(z, y, x)] = value;
    }
    const size3d& size() const
    {
        return size_;
    }

private:
    std::size_t idx(std::size_t z, std::size_t y, size_t x) const
    {
        return z * size().height() * size().width() + y * size().width() + x;
    };
    size3d size_;
    std::vector<float> values_;
};

std::string show_matrix3d(const matrix3d& m);
