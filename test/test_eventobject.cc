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
#include <opencv2/core/mat.hpp>
#include "util/config.h"
#include "event_object.h"


using namespace std;
using namespace cv;
using namespace deepsea;

int main()
{
    ofstream cfg_file;
    cfg_file.open("config.json");
    cfg_file << "{" << endl;
    cfg_file << "\"program\": \"mbariviz\"," << endl;
    cfg_file << "\"tracker\": 0," << endl;
    cfg_file << "\"min_event_frames\": 3," << endl;
    cfg_file << "\"kalman_pnoise\": 0.1," << endl;
    cfg_file << "\"kalman_mnoise\": 10.0" << endl;
    cfg_file << "}" << endl;
    cfg_file.close();

    Config cfg("config.json");
    assert(cfg.isInitialized());

    const Rect bbox(0, 0, 0, 0);
    const unsigned int occluded_pixels = 0;
    const unsigned int frame = 0;
    VOCObject voc;
    voc.setName("Unknown");
    voc.setConfidence(1.0);
    voc.setBox(Rect(0,0,100,100));
    voc.setIndex(1.0);
    EventObject object(voc, occluded_pixels, frame);
}