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

#include "include/deepsea/util/ConfigClassMap.h"

#include <iostream>
#include <string>

using namespace std;
using namespace avedac;

int main()
{
    string filename = "/Users/dcline/Sandbox/deepsea-track/deepsea_class_map.json";
    try {
        ConfigMaps cfg;
        initConfigMaps(filename, cfg);
        cout << cfg.animal_track_ids["one"] << endl;
        cout << cfg.animal_track_ids["two"] << endl;
    } catch(exception&) {
        cout << "error" << endl;
    }
}