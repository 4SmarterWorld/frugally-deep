// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include <iostream>

#include "frugally_deep/frugally_deep.h"

#include "frugally_deep/json.hpp"

#include <fplus/fplus.hpp>

namespace fd
{

inline fd::size3d create_size3d(const nlohmann::json& data)
{
    fd::assertion(data.is_array(), "size3d needs to be an array");
    fd::assertion(data.size() > 0, "need at least one dimension");
    const std::size_t offset = data[0].is_null() ? 1 : 0;
    if (data.size() == 1 + offset)
        return fd::size3d(0, 0, data[0 + offset]);
    if (data.size() == 2 + offset)
        return fd::size3d(0, data[0 + offset], data[1 + offset]);
    if (data.size() == 3 + offset)
        return fd::size3d(data[0 + offset], data[1 + offset], data[2 + offset]);
    fd::raise_error("size3d needs 1, 2 or 3 dimensions");
    return fd::size3d(0, 0, 0);
}

inline fd::size2d create_size2d(const nlohmann::json& data)
{
    fd::assertion(data.is_array(), "size2d needs to be an array");
    if (data.size() == 1)
        return fd::size2d(0, data[0]);
    if (data.size() == 2)
        return fd::size2d(data[0], data[1]);
    fd::raise_error("size2d needs 1 or 2 dimensions");
    return fd::size2d(0, 0);
}

inline fd::matrix3d create_matrix3d(const nlohmann::json& data)
{
    const fd::size3d shape = create_size3d(data["shape"]);
    const fd::float_vec values = data["values"];
    return fd::matrix3d(shape, values);
}

template <typename T, typename F>
std::vector<T> create_vector(F f, const nlohmann::json& data)
{
    fd::assertion(data.is_array(), "array needs to be an array");
    return fplus::transform_convert<std::vector<T>>(f, data);
}

inline fd::float_t create_singleton_vec(const nlohmann::json& data)
{
    fd::float_vec values = data;
    fd::assertion(values.size() == 1, "need exactly one value");
    return values[0];
}

using get_param_f =
    std::function<fd::float_vec(const std::string&, const std::string&)>;

fd::model create_model(const get_param_f& get_param, const nlohmann::json& data);

inline fd::layer_ptr create_model_layer(
    const get_param_f& get_param, const nlohmann::json& data)
{
    return std::make_shared<fd::model>(create_model(get_param, data));
}

inline void fill_with_zeros(fd::float_vec& xs)
{
    std::fill(std::begin(xs), std::end(xs), 0);
}

inline fd::layer_ptr create_conv2d_layer(
    const get_param_f& get_param, const nlohmann::json& data)
{
    const std::string name = data["name"];
    fd::float_vec bias = get_param(name, "bias");
    const bool use_bias = data["config"]["use_bias"];
    if (!use_bias) fill_with_zeros(bias);
    const std::string padding_str = data["config"]["padding"];
    const auto maybe_padding =
        fplus::choose<std::string, fd::convolutional_layer::padding>({
        { std::string("valid"), fd::convolutional_layer::padding::valid },
        { std::string("same"), fd::convolutional_layer::padding::same },
    }, padding_str);
    fd::assertion(fplus::is_just(maybe_padding), "no padding");
    const auto padding = maybe_padding.unsafe_get_just();

    const fd::size2d strides = create_size2d(data["config"]["strides"]);

    fd::assertion(strides.width_ == strides.height_,
        "strides not proportional");

    const std::size_t filter_count = data["config"]["filters"];
    fd::assertion(bias.size() == filter_count, "size of bias does not match");

    const fd::float_vec weights = get_param(name, "weights");
    const fd::size2d kernel_size = create_size2d(data["config"]["kernel_size"]);
    fd::assertion(weights.size() % kernel_size.area() == 0,
        "invalid number of weights");
    const std::size_t filter_depths =
        weights.size() / (kernel_size.area() * filter_count);
    const fd::size3d filter_size(
        filter_depths, kernel_size.height_, kernel_size.width_);

    return std::make_shared<fd::convolutional_layer>(name,
        filter_size, filter_count, strides, padding, weights, bias);
}

inline fd::layer_ptr create_input_layer(
    const get_param_f&, const nlohmann::json& data)
{
    assertion(data["inbound_nodes"].empty(),
        "input layer is not allowed to have inbound nodes");
    const std::string name = data["name"];
    const auto input_shape = create_size3d(data["config"]["batch_input_shape"]);
    return std::make_shared<fd::input_layer>(name, input_shape);
}

inline fd::layer_ptr create_batch_normalization_layer(
    const get_param_f& get_param, const nlohmann::json& data)
{
    const std::string name = data["name"];
    fd::float_t epsilon = data["config"]["epsilon"];
    const bool center = data["config"]["center"];
    const bool scale = data["config"]["scale"];
    fd::float_vec gamma;
    fd::float_vec beta;
    if (scale) gamma = get_param(name, "gamma");
    if (center) beta = get_param(name, "beta");
    return std::make_shared<fd::batch_normalization_layer>(
        name, epsilon, beta, gamma);
}

inline fd::layer_ptr create_dropout_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    // dropout rate equals zero in forward pass
    return std::make_shared<fd::identity_layer>(name);
}

inline fd::layer_ptr create_leaky_relu_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    fd::float_t alpha = data["config"]["alpha"];
    return std::make_shared<fd::leaky_relu_layer>(name, alpha);
}

inline fd::layer_ptr create_elu_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    fd::float_t alpha = data["config"]["alpha"];
    return std::make_shared<fd::elu_layer>(name, alpha);
}

inline fd::layer_ptr create_max_pooling2d_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    const auto pool_size = create_size2d(data["config"]["pool_size"]);
    const auto strides = create_size2d(data["config"]["strides"]);
    // todo: support pool_size != strides
    fd::assertion(pool_size == strides, "pool_size not strides equal");
    // todo: support non-proportional sizes
    fd::assertion(pool_size.width_ == pool_size.height_,
        "pooling not proportional");
    return std::make_shared<fd::max_pool_layer>(name, pool_size.width_);
}

inline fd::layer_ptr create_average_pooling2d_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    const auto pool_size = create_size2d(data["config"]["pool_size"]);
    const auto strides = create_size2d(data["config"]["strides"]);
    // todo: support pool_size != strides
    fd::assertion(pool_size == strides, "pool_size not strides equal");
    // todo: support non-proportional sizes
    fd::assertion(pool_size.width_ == pool_size.height_,
        "pooling not proportional");
    return std::make_shared<fd::avg_pool_layer>(name, pool_size.width_);
}

inline fd::layer_ptr create_upsampling2d_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    const auto size = create_size2d(data["config"]["size"]);
    fd::assertion(size.width_ == size.height_, "invalid scale factor");
    return std::make_shared<fd::unpool_layer>(name, size.width_);
}

inline fd::layer_ptr create_dense_layer(
    const get_param_f& get_param, const nlohmann::json& data)
{
    const std::string name = data["name"];
    const fd::float_vec weights = get_param(name, "weights");
    fd::float_vec bias = get_param(name, "bias");
    const bool use_bias = data["config"]["use_bias"];
    if (!use_bias) fill_with_zeros(bias);
    std::size_t units = data["config"]["units"];
    fd::assertion(bias.size() == units, "invalid bias count");
    fd::assertion(weights.size() % units == 0, "invalid weight count");
    const std::size_t size_in = weights.size() / units;
    return std::make_shared<fd::fully_connected_layer>(name, size_in, units);
}

inline fd::layer_ptr create_concatename_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    return std::make_shared<fd::concatenate_layer>(name);
}

inline fd::layer_ptr create_flatten_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    return std::make_shared<fd::flatten_layer>(name);
}

inline fd::activation_layer_ptr create_identity_layer(const std::string& name)
{
    return std::make_shared<fd::identity_layer>(name);
}

inline fd::activation_layer_ptr create_softmax_layer(const std::string& name)
{
    return std::make_shared<fd::softmax_layer>(name);
}

inline fd::activation_layer_ptr create_softplus_layer(const std::string& name)
{
    return std::make_shared<fd::softplus_layer>(name);
}

inline fd::activation_layer_ptr create_tanh_layer(const std::string& name)
{
    return std::make_shared<fd::tanh_layer>(name);
}

inline fd::activation_layer_ptr create_sigmoid_layer(const std::string& name)
{
    return std::make_shared<fd::sigmoid_layer>(name);
}

inline fd::activation_layer_ptr create_hard_sigmoid_layer(
    const std::string& name)
{
    return std::make_shared<fd::hard_sigmoid_layer>(name);
}

inline fd::activation_layer_ptr create_relu_layer(const std::string& name)
{
    return std::make_shared<fd::relu_layer>(name);
}

inline fd::activation_layer_ptr create_selu_layer(const std::string& name)
{
    return std::make_shared<fd::selu_layer>(name);
}

inline fd::activation_layer_ptr create_activation_layer(
    const std::string& type, const std::string& name)
{
    const std::unordered_map<std::string,
        std::function<fd::activation_layer_ptr(const std::string&)>>
    creators = {
        {"linear", create_identity_layer},
        {"softmax", create_softmax_layer},
        {"softplus", create_softplus_layer},
        {"tanh", create_tanh_layer},
        {"sigmoid", create_sigmoid_layer},
        {"hard_sigmoid", create_hard_sigmoid_layer},
        {"relu", create_relu_layer},
        {"selu", create_selu_layer}
    };

    return fplus::throw_on_nothing(
        fd::error("unknown activation type: " + type),
        fplus::get_from_map(creators, type))(name);
}

inline fd::layer_ptr create_activation_layer_as_layer(
    const get_param_f&, const nlohmann::json& data)
{
    const std::string name = data["name"];
    const std::string type = data["config"]["activation"];
    return create_activation_layer(type, name);
}

inline bool json_obj_has_member(const nlohmann::json& data,
    const std::string& member_name)
{
    return data.is_object() && data.find(member_name) != data.end();
}

node_connection create_node_connection(const nlohmann::json& data)
{
    fd::assertion(data.is_array(), "invalid format for inbound node");
    const std::string layer_id = data[0];
    const std::size_t node_idx = data[1];
    const std::size_t tensor_idx = data[2];
    return node_connection(layer_id, node_idx, tensor_idx);
}

node create_node(const nlohmann::json& inbound_nodes_data)
{
    fd::assertion(inbound_nodes_data.is_array(), "nodes need to be an array");
    return node(create_vector<node_connection>(create_node_connection,
            inbound_nodes_data));
}

nodes create_nodes(const nlohmann::json& data)
{
    fd::assertion(data["inbound_nodes"].is_array(), "no inbound nodes");
    const std::vector<nlohmann::json> inbound_nodes_data = data["inbound_nodes"];
    return fplus::transform(create_node, inbound_nodes_data);
}

inline fd::layer_ptr create_layer(
    const get_param_f& get_param, const nlohmann::json& data)
{
    const std::string name = data["name"];

    const std::unordered_map<std::string,
            std::function<fd::layer_ptr(const get_param_f&, const nlohmann::json&)>>
        creators = {
            {"Model", create_model_layer},
            {"Conv2D", create_conv2d_layer},
            {"InputLayer", create_input_layer},
            {"BatchNormalization", create_batch_normalization_layer},
            {"Dropout", create_dropout_layer},
            {"LeakyReLU", create_leaky_relu_layer},
            {"ELU", create_elu_layer},
            {"MaxPooling2D", create_max_pooling2d_layer},
            {"AveragePooling2D", create_average_pooling2d_layer},
            {"UpSampling2D", create_upsampling2d_layer},
            {"Dense", create_dense_layer},
            {"Concatenate", create_concatename_layer},
            {"Flatten", create_flatten_layer},
            {"Activation", create_activation_layer_as_layer}
        };

    const std::string type = data["class_name"];

    auto result = fplus::throw_on_nothing(
        fd::error("unknown layer type: " + type),
        fplus::get_from_map(creators, type))(get_param, data);

    if (json_obj_has_member(data["config"], "activation"))
    {
        result->set_activation(
            create_activation_layer(data["config"]["activation"], ""));
    }

    result->set_nodes(create_nodes(data));

    return result;
}

node create_input_node_from_connection(const node_connection& connection)
{
    assertion(connection.node_idx_ == 0, "invalid input node connection");
    return node(node_connections({connection}));
}

inline fd::model create_model(const get_param_f& get_param,
    const nlohmann::json& data)
{
    //output_nodes
    //input_nodes
    const std::string name = data["config"]["name"];

    fd::assertion(data["config"]["layers"].is_array(), "missing layers array");

    const auto layers = create_vector<layer_ptr>(
        fplus::bind_1st_of_2(create_layer, get_param),
        data["config"]["layers"]);

    // todo: remove
    const auto show_layer = [](const fd::layer_ptr& ptr) -> std::string
    {
        return ptr->name_;
    };
    std::cout
        << fplus::show_cont_with("\n", fplus::transform(show_layer, layers))
            << std::endl;

    fd::assertion(data["config"]["input_layers"].is_array(), "no input layers");

    const auto input_nodes = fplus::transform(create_input_node_from_connection,
        create_vector<node_connection>(
            create_node_connection, data["config"]["input_layers"]));

    const auto outputs = create_vector<node_connection>(
        create_node_connection, data["config"]["output_layers"]);

    fd::model result(name, layers, outputs);
    return result;
}

struct test_case
{
    fd::matrix3ds input_;
    fd::matrix3ds output_;
};

using test_cases = std::vector<test_case>;

inline test_case load_test_case(const nlohmann::json& data)
{
    fd::assertion(data["inputs"].is_array(), "test needs inputs");
    fd::assertion(data["outputs"].is_array(), "test needs outputs");
    return {
        create_vector<fd::matrix3d>(create_matrix3d, data["inputs"]),
        create_vector<fd::matrix3d>(create_matrix3d, data["outputs"])
    };
}

inline test_cases load_test_cases(const nlohmann::json& data)
{
    fd::assertion(data["tests"].is_array(), "no tests");
    return fplus::transform_convert<test_cases>(
        load_test_case, data["tests"]);
}

inline bool run_test_cases(const fd::model& model, const test_cases& tests)
{
    for (const auto& test_case : tests)
    {
        const auto output = model.predict(test_case.input_);
        if (output != test_case.output_)
            return false;
    }
    return true;
}

// Throws an exception if a problem occurs.
inline fd::model load_model(const std::string& path, bool verify = true)
{
    const auto maybe_json_str = fplus::read_text_file_maybe(path)();
    fd::assertion(fplus::is_just(maybe_json_str), "Unable to load: " + path);

    const auto json_str = maybe_json_str.unsafe_get_just();
    const auto json_data = nlohmann::json::parse(json_str);

    const std::function<fd::float_vec(const std::string&, const std::string&)>
        get_param = [&json_data]
        (const std::string& layer_name, const std::string& param_name)
        -> fd::float_vec
    {
        const fd::float_vec result =
            json_data["trainable_params"][layer_name][param_name];
        return result;
    };

    const auto model = create_model(get_param, json_data["architecture"]);

    const auto tests = load_test_cases(json_data);

    if (verify)
    {
        fd::assertion(run_test_cases(model, tests), "Tests failed.");
    }

    return model;
}

} // namespace fd


// todo replace transform_convert with create_vector

// todo class node gucken https://github.com/fchollet/keras/blob/master/keras/engine/topology.py