#!/usr/bin/env python3
"""Save application models mentioned in Keras documentation
"""

import os
import sys

import numpy as np

import keras

__author__ = "Tobias Hermann"
__copyright__ = "Copyright 2017, Tobias Hermann"
__license__ = "MIT"
__maintainer__ = "Tobias Hermann, https://github.com/Dobiasd/frugally-deep"
__email__ = "editgym@gmail.com"

def save_xception(dir_path, file_name):
    keras.applications.xception.Xception(
        include_top=True, weights='imagenet', input_tensor=None,
        input_shape=None, pooling=None, classes=1000).save(
            os.path.join(dir_path, file_name + ".h5"))

def save_vgg16(dir_path, file_name):
    keras.applications.vgg16.VGG16(
        include_top=True, weights='imagenet', input_tensor=None,
        input_shape=None, pooling=None, classes=1000).save(
            os.path.join(dir_path, file_name + ".h5"))

def save_vgg19(dir_path, file_name):
    keras.applications.vgg19.VGG19(
        include_top=True, weights='imagenet',input_tensor=None,
        input_shape=None, pooling=None, classes=1000).save(
            os.path.join(dir_path, file_name + ".h5"))

def save_resnet50(dir_path, file_name):
    keras.applications.resnet50.ResNet50(
        include_top=True, weights='imagenet', input_tensor=None,
        input_shape=None, pooling=None, classes=1000).save(
            os.path.join(dir_path, file_name + ".h5"))

def save_inceptionv3(dir_path, file_name):
    keras.applications.inception_v3.InceptionV3(
        include_top=True, weights='imagenet', input_tensor=None,
        input_shape=None, pooling=None, classes=1000).save(
            os.path.join(dir_path, file_name + ".h5"))

def save_inceptionvresnetv2(dir_path, file_name):
    keras.applications.inception_resnet_v2.InceptionResNetV2(
        include_top=True, weights='imagenet', input_tensor=None,
        input_shape=None, pooling=None, classes=1000).save(
            os.path.join(dir_path, file_name + ".h5"))

def save_mobilenet(dir_path, file_name):
    keras.applications.mobilenet.MobileNet(
        input_shape=None, alpha=1.0, depth_multiplier=1,
        dropout=1e-3, include_top=True, weights='imagenet',
        input_tensor=None, pooling=None, classes=1000).save(
            os.path.join(dir_path, file_name + ".h5"))

def main():
    if len(sys.argv) != 2:
        print('usage: [output path]')
        sys.exit(1)
    else:
        dir_path = sys.argv[1]
        save_xception(dir_path, 'xception')
        save_vgg16(dir_path,'vgg16')
        save_vgg19(dir_path, 'vgg19')
        save_resnet50(dir_path, 'resnet50')
        save_inceptionv3(dir_path, 'inceptionv3')
        #save_inceptionvresnetv2(dir_path, 'inceptionvresnetv2') # wait for pip
        save_mobilenet(dir_path, 'mobilenet')

        #keras_export/export_model.py keras_export/xception.h5 keras_export/xception.json
        #keras_export/export_model.py keras_export/vgg16.h5 keras_export/vgg16.json
        #keras_export/export_model.py keras_export/vgg19.h5 keras_export/vgg19.json
        #keras_export/export_model.py keras_export/resnet50.h5 keras_export/resnet50.json
        #keras_export/export_model.py keras_export/inceptionv3.h5 keras_export/inceptionv3.json
        #keras_export/export_model.py keras_export/inceptionvresnetv2.h5 keras_export/inceptionvresnetv2.json
        #keras_export/export_model.py keras_export/mobilenet.h5 keras_export/mobilenet.json

if __name__ == "__main__":
    main()
