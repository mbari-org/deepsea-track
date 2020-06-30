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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include <opencv2/opencv.hpp>

using namespace std;

namespace deepsea {

    struct ConfigMaps {
        map<string, int> class_ids;
        map<int, string> class_descriptions;
        map<int, cv::Scalar> class_colors;
    };

    void from_json(const nlohmann::json& j, ConfigMaps& p);

    bool initConfigMaps(string filename, ConfigMaps& cfg);

} // namespace deepsea