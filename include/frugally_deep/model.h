// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/typedefs.h"

#include "frugally_deep/matrix3d.h"

#include "frugally_deep/node.h"

#include <cstddef>
#include <memory>

namespace fd
{

class model : public layer
{
public:
    explicit model(const std::string& name)
        : layer(name)
    {
    }
    virtual ~model()
    {
    }
    virtual matrix3ds apply(const matrix3ds& input) const override
    {
        // todo
        return input;
    }
private:
    using node_id = std::string;
    using node_ids = std::vector<node_id>;

    std::unordered_map<node_id, node> nodes;
    node_ids outputs;
    node_ids inputs;
};

} // namespace fd
