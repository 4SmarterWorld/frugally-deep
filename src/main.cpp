// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#include "frugally_deep/frugally_deep.hpp"

int main()
{
    std::vector<std::string> model_paths = {
        "keras_export/test_model.json",
        "keras_export/xception.json",
        "keras_export/vgg16.json",
        "keras_export/vgg19.json",
        "keras_export/resnet50.json",
        "keras_export/inceptionv3.json"
        //"keras_export/inceptionvresnetv2.json", // wait for pip
        //"keras_export/mobilenet.json" // relu6
    };

    bool error = false;
    for (const auto& model_path : model_paths)
    {
        try
        {
            const auto model = fdeep::load_model(model_path);
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
            error = true;
        }
    }

    if (error)
    {
        std::cout << "There were errors." << std::endl;
        return 1;
    }
    std::cout << "All imports and test OK." << std::endl;
}
