// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#include "tests/cifar10.h"
#include "tests/gradient_check.h"
#include "tests/gradient_images.h"
#include "tests/lenna_filter.h"
#include "tests/mnist.h"
#include "tests/test_helpers.h"
#include "tests/xor.h"
#include "tests/variance_inflation_training.h"

#include "frugally_deep/frugally_deep.h"

#include <fplus/fplus.h>

#include <cassert>
#include <fstream>
#include <iostream>

int main()
{
    variance_inflation_training_test();
    return 0;
    lenna_filter_test();
    gradient_check_backprop_implementation();
    xor_as_net_test();
    gradients_classification_test();
    mnist_classification_test();
    cifar_10_classification_test();
}
