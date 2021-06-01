#pragma once
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
///\file Utils.H methods for general image functions not included in opencv

#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <nlohmann/json.hpp>            // json
#include <sys/stat.h>

using namespace std;
using namespace cv;
using namespace nlohmann;

namespace deepsea {
    class Utils {

    public:
        /// \brief Empty contructor
        Utils() {};

        /// \brief destructor
        virtual ~Utils();

        /// \brief computes IOU between two bounding boxes
        /// \param bb1
        /// \param bb2
        /// \return intersection over union
        static double iou(const Rect_<float> bb1, const Rect_<float> bb2);

        /// \brief rescales bounding box
        /// \param resize_factor_width factor to rescale width
        /// \param resize_factor_height factor to rescale height
        /// \param bbox
        /// \return scaled bounding box
        static Rect2d rescale(float resize_factor_width, float resize_factor_height, const Rect2d &bbox);

        /// \brief convert bounding box rectangle to json
        /// \param[in] box to convert
        /// \return json object with encoded box variables x,y,width,height
        static json rect2json(const Rect2d &box);

        /// \brief convert json to rectangle
        /// \param[in] j json object
        /// \return rectangle with decoded box variables x,y,width,height
        static Rect2d json2rect(const json &j);

        /// \brief decorate frame with colored box and descriptive strings
        /// \param frame frame to decorate
        /// \param bbox bounding box in frame coordinates
        /// \param color RGB color for the box
        /// \param uuid string unique identifier for the object in the box
        /// \param description  description for the object in the box
        /// \param thickness_box thickness of box
        /// \param font_scale scale of the font
        static void decorate(Mat frame, const Rect2d bbox, const Scalar color,  const string uuid,
                const string description, int thickness_box=5, float font_scale=0.70);

        /// Simple file check to check if a file exists
        /// \param[in] file_name  filename with full path to file
        /// \return true if exists
        static bool doesPathExist(const string &file_name)
        {
            struct stat buffer;
            return (stat (file_name.c_str(), &buffer) == 0);
        }
    };
}