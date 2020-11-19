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

#include <onnxruntime/core/session/experimental_onnxruntime_cxx_api.h>
#include <cmath>
#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <atomic>
#include <stdlib.h>

// pretty prints a shape dimension vector
std::string print_shape(const std::vector<int64_t>& v) {
    std::stringstream ss("");
    for (size_t i = 0; i < v.size() - 1; i++)
        ss << v[i] << "x";
    ss << v[v.size() - 1];
    return ss.str();
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
    if (argc != 2) {
        cout << "Usage: ./test_detmodel_load <onnx_model.onnx> " << endl;
        return -1;
    }
    // TODO: need to take care of the following line as it is related to CPU
    // consumption using openmp
    // session_options.SetIntraOpNumThreads(1);
#ifdef USE_CUDA
     #include "cuda_provider_factory.h"
     OrtSessionOptionsAppendExecutionProvider_CUDA(session_options, 1);
#endif
    std::string model_file = argv[1];

    try {

        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "Testing Onnxruntime Model Load");
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
        for (int64_t x : input_shape)
            cout << x << " " ;

        std::vector<std::string> output_names = session.GetOutputNames();
        std::vector<std::vector<int64_t> > output_shapes = session.GetOutputShapes();
        cout << "Output Node Name/Shape (" << output_names.size() << "):" << endl;
        for (size_t i = 0; i < output_names.size(); i++) {
            cout << "\t" << output_names[i] << " : " << print_shape(output_shapes[i]) << endl;
        }


    } catch (const Ort::Exception& exception) {
        cout << "ERROR loading model : " << exception.what() << endl;
        exit(-1);
    }
}