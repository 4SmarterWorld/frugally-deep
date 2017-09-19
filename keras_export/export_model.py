import json
import sys

from keras.models import load_model
from keras import backend as K

import numpy as np

def write_text_file(path, text):
    with open(path, "w") as text_file:
        print(text, file=text_file)

def arr_as_arr3(arr):
    assert arr.shape[0] == 1
    depth = len(arr.shape)
    if depth == 2:
        return arr.reshape(1, 1, *arr.shape[1:])
    if depth == 3:
        return arr.reshape(1, *arr.shape[1:])
    if depth == 4:
        return arr.reshape(arr.shape[1:])
    else:
        raise ValueError('invalid number of dimensions')

def show_tensor3(tens):
    return {
        'shape': tens.shape,
        'values': tens.flatten().tolist()
    }

def show_test_data_as_3tensor(arr):
    return show_tensor3(arr_as_arr3(arr))

def gen_test_data(model):
    data_in = list(map(lambda l: np.random.random((1, *l.input_shape[1:])),
        model.input_layers))
    data_out = model.predict(data_in)
    return {
        'inputs': list(map(show_test_data_as_3tensor, data_in)),
        'outputs': list(map(show_test_data_as_3tensor, data_out))
    }

def show_conv2d_layer(layer):
    weights = layer.get_weights()
    weight_flat = np.swapaxes(weights[0], 0, 2).flatten().tolist()
    assert len(weight_flat) > 0
    assert layer.dilation_rate == (1,1)
    assert layer.padding in ['valid', 'same']
    assert len(layer.input_shape) == 4
    assert layer.input_shape[0] == None
    return {
        'weights': weight_flat,
        'biases': weights[1].tolist()
    }

def show_batch_normalization_layer(layer):
    assert layer.axis == -1
    return {
        'gamma': K.get_value(layer.gamma).tolist(),
        'beta': K.get_value(layer.beta).tolist()
        }

def show_dense_layer(layer):
    assert len(layer.input_shape) == 2, "Please flatten for dense layer."
    assert layer.input_shape[0] == None, "Please flatten for dense layer."
    assert layer.use_bias == True
    assert layer.kernel_constraint == None
    assert layer.bias_constraint == None
    weights, bias = layer.get_weights()
    return {
        'weights': weights.flatten().tolist(),
        'bias': bias.tolist()
        }

def get_dict_keys(d):
    return [key for key in d]

def merge_two_dicts(x, y):
    """Given two dicts, merge them into a new dict as a shallow copy."""
    z = x.copy()
    z.update(y)
    return z

def merge_two_disjunct_dicts(x, y):
    assert set(get_dict_keys(x)).isdisjoint(get_dict_keys(y))
    return merge_two_dicts(x, y)

def is_ascii(str):
    try:
        str.encode('ascii')
    except UnicodeEncodeError:
        return False
    else:
        return True

def get_all_weights(model):
    show_layer_functions = {
        'Conv2D': show_conv2d_layer,
        'BatchNormalization': show_batch_normalization_layer,
        'Dense': show_dense_layer
    }
    result = {}
    layers = model.layers
    for layer in layers:
        layer_type = type(layer).__name__
        if layer_type in ['Model', 'Sequential']:
            result = merge_two_disjunct_dicts(result, get_all_weights(layer))
        else:
            show_func = show_layer_functions.get(layer_type, None)
            name = layer.name
            is_ascii(name)
            if name in result:
                raise ValueError('duplicate layer name ' + name)
            if show_func:
                result[name] = show_func(layer)
    return result

def main():
    usage = 'usage: [Keras model in HDF5 format] [output path] [test count = 3]'
    if len(sys.argv) != 3 and len(sys.argv) != 4:
        print(usage)
        sys.exit(1)
    else:
        in_path = sys.argv[1]
        out_path = sys.argv[2]
        test_count = 3
        if len(sys.argv) == 4:
            test_count = int(sys.argv[3])
        model = load_model(in_path)

        # todo remove
        write_text_file(out_path + '.keras.yml', model.to_yaml()) 

        # todo remove
        write_text_file(out_path + '.keras.json', json.dumps(
                model.to_json(), allow_nan=False, indent=2, sort_keys=True))

        json_output = {}

        json_output['architecture'] = json.loads(model.to_json())

        json_output['trainable_params'] = get_all_weights(model)

        json_output['tests'] = [gen_test_data(model) for _ in range(test_count)]

        write_text_file(out_path, json.dumps(
                json_output, allow_nan=False, indent=2, sort_keys=True))

if __name__ == "__main__":
    main()
