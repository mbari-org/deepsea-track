/*
 * Copyright 2020 MBARI
 *
 * Licensed under the GNU LESSER GENERAL PUBLIC LICENSE, Version 3.0
 * (the "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 * http://www.gnu.org/copyleft/lesser.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This work would not be possible without the generous support of the
 * David and Lucile Packard Foundation
 */

#include <opencv2/opencv.hpp>
#include <core/session/experimental_onnxruntime_cxx_api.h>
#include <array>
#include <cmath>
#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <atomic>
#include <stdlib.h>
#include <algorithm>  // std::generate
#include <assert.h>
#include <iostream>
#include <sstream>
#include <vector> // std::vector

// pretty prints a shape dimension vector
std::string print_shape(const std::vector<int64_t>& v) {
    std::stringstream ss("");
    for (size_t i = 0; i < v.size() - 1; i++)
        ss << v[i] << "x";
    ss << v[v.size() - 1];
    return ss.str();
}

int calculate_product(const std::vector<int64_t>& v) {
    int total = 1;
    for (auto& i : v) total *= i;
    return total;
}

std::string toString(const ONNXTensorElementDataType dataType)
{
    switch (dataType) {
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT: {
            return "float";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8: {
            return "uint8_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8: {
            return "int8_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16: {
            return "uint16_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16: {
            return "int16_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32: {
            return "int32_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64: {
            return "int64_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING: {
            return "string";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL: {
            return "bool";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16: {
            return "float16";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE: {
            return "double";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32: {
            return "uint32_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64: {
            return "uint64_t";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX64: {
            return "complex with float32 real and imaginary components";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX128: {
            return "complex with float64 real and imaginary components";
        }
        case ONNX_TENSOR_ELEMENT_DATA_TYPE_BFLOAT16: {
            return "complex with float64 real and imaginary components";
        }
        default:
            return "undefined";
    }
}

using namespace std;

int main(int argc, char** argv) {
    if (argc != 3) {
        cout << "Usage: ./model-explorer <onnx_model.onnx> <image name>" << endl;
        return -1;
    }
    // TODO: need to take care of the following line as it is related to CPU
    // consumption using openmp
//    session_options.SetIntraOpNumThreads(1);
//
//#ifdef USE_CUDA
//    // #include "cuda_provider_factory.h"
//    // OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 1);
//#endif
    std::string model_file = argv[1];
    const std::string image_file = argv[2];
    cv::Mat img = cv::imread(image_file);

    if (img.empty()) {
        std::cerr << "Failed to read input image" << std::endl;
        return -1;
    }
    // onnxruntime setup
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "Testing EfficientDet Onnxruntime Model");
    Ort::SessionOptions session_options;
    Ort::Experimental::Session session = Ort::Experimental::Session(env, model_file, session_options);

    std::vector<std::string> input_names = session.GetInputNames();
    std::vector<std::vector<int64_t> > input_shapes = session.GetInputShapes();
    cout << "Input Node Name/Shape (" << input_names.size() << "):" << endl;
    for (size_t i = 0; i < input_names.size(); i++) {
        cout << "\t" << input_names[i] << " : " << print_shape(input_shapes[i]) << endl;
    }
    auto shape = input_shapes[0];
    vector<int64_t> input_shape;
    input_shape.push_back(1);
    input_shape.push_back(shape[1]); // width
    input_shape.push_back(shape[2]); // height
    input_shape.push_back(shape[3]); // depth
    int64_t width = shape[1];
    int64_t height = shape[2];
    int64_t depth = shape[3];
    for (int64_t x : input_shape)
        cout << x << " " ;

    // print name/shape of outputs
    std::vector<std::string> output_names = session.GetOutputNames();
    std::vector<std::vector<int64_t> > output_shapes = session.GetOutputShapes();
    cout << "Output Node dName/Shape (" << output_names.size() << "):" << endl;
    for (size_t i = 0; i < output_names.size(); i++) {
        cout << "\t" << output_names[i] << " : " << print_shape(output_shapes[i]) << endl;
    }

    // Create a single Ort tensor with the number of elements
    int total_number_elements = calculate_product(input_shape);
    std::vector<float> input_tensor_values(width*height*depth);

    // Normalize to 0-1 and subtract mean/std
    // assume 3 color RGB  - do we need to swap color channels here?
    std::vector<float> mean{0.41764800491508236, 0.4998548971956831, 0.40066758991524853};
    std::vector<float> stdev{0.007470813932609973, 0.006994665000683959, 0.0070365565871661675};
    for (int c = 0; c < 3; ++c) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < width; ++j) {
                input_tensor_values[c * height * width + i * width + j] =  (img.data[i * width * 3 + j * 3 + c] / 255.0) - mean[c] - stdev[c];
            }
        }
    }

    // generate random numbers in the range [0, 255]
    std::vector<Ort::Value> input_tensors;
    input_tensors.push_back(Ort::Experimental::Value::CreateTensor<float>(input_tensor_values.data(),
                                                                          input_tensor_values.size(), input_shape));

    // pass data through model
    cout << "Running model...";
    try {
        auto output_tensors = session.Run(session.GetInputNames(), input_tensors, session.GetOutputNames());
        cout << "done" << endl;
        cout << "output_tensor_shape: " << print_shape(output_tensors[0].GetTensorTypeAndShapeInfo().GetShape()) << endl;
        int64_t num_detections = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape()[1];
        int64_t num_bboxes = output_tensors[0].GetTensorTypeAndShapeInfo().GetShape()[0];
        cout << "num boxes: " << num_bboxes << " num detections: " << num_detections << endl << endl;

        cout << "output_tensor_classes: " << print_shape(output_tensors[1].GetTensorTypeAndShapeInfo().GetShape()) << endl;
        int64_t num_classes = output_tensors[1].GetTensorTypeAndShapeInfo().GetShape()[1];
        cout << "num_classes: " << num_classes << endl << endl;

        cout << "output_tensor_scores: " << print_shape(output_tensors[2].GetTensorTypeAndShapeInfo().GetShape()) << endl;
        int64_t num_scores = output_tensors[2].GetTensorTypeAndShapeInfo().GetShape()[1];
        cout << "num_scores: " << num_scores << endl << endl;

        std::vector<float> scores;
        std::vector<uint64_t> indices;
        std::vector<std::array<float, 4>> bboxes;
        float ratioW = 1.0f; //placeholder
        float ratioH = 1.0f; //placeholder
        float* boxes_float = output_tensors[0].GetTensorMutableData<float>();
        float* label_indices_float = output_tensors[1].GetTensorMutableData<float>();
        float* scores_float = output_tensors[2].GetTensorMutableData<float>();
        size_t index = 0;
        while (index < num_detections) {
            for (int i = 0; i < 100; i++) {
                float xmin = boxes_float[0];
                float ymin = boxes_float[1];
                float xmax = boxes_float[2];
                float ymax = boxes_float[3];
                bboxes.emplace_back(std::array<float, 4>{xmin * ratioW, ymin * ratioH, xmax * ratioW, ymax * ratioH});
                index += 1;
            }
            scores.emplace_back(scores_float[index]);
            indices.emplace_back(label_indices_float[index]);
        }


    } catch (const Ort::Exception& exception) {
        cout << "ERROR loading model : " << exception.what() << endl;
        exit(-1);
    }
}