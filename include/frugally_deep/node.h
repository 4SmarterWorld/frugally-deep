// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "frugally_deep/layers/layer.h"

#include <cstddef>
#include <memory>

namespace fd
{

struct node_id
{
    std::string layer_id_;
    std::size_t node_idx_;
};
using node_ids = std::vector<node_id>;

class node
{
public:
    explicit node(
        const node_ids& inbound_nodes,
        const layer_ptr& output_layer) :
            inbound_nodes_(inbound_nodes),
            outbound_layer_(output_layer),
            outputs_()
    {
    }
    matrix3d get_output(std::size_t) const
    {
        // todo
        return matrix3d(size3d(0,0,0), {});
    }
    void set_outputs(const matrix3ds& outputs)
    {
        // todo: only for nodes of input-layers?
        outputs_ = fplus::just<matrix3ds>(outputs);
    }
private:
    node_ids inbound_nodes_;
    layer_ptr outbound_layer_;
    fplus::maybe<matrix3ds> outputs_;
};

} // namespace fd
