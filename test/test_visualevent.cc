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
#include "event_object.h"
#include "visual_event.h"

using namespace std;
using namespace cv;
using namespace boost;
using namespace deepsea;

int main()
{
    ofstream c;
    c.open("config.json");
    c << "{" << endl;
    c << "\"program\": \"test_visual_event\"," << endl;
    c << "\"tracker\": 0," << endl;
    c << "\"min_event_frames\": 3," << endl;
    c << "\"kalman_pnoise\": 0.1," << endl;
    c << "\"kalman_mnoise\": 10.0" << endl;
    c << "}" << endl;
    c.close();

    c.open("config_map.json");
    c << "{" << endl;
    c << "\"items\": [" << endl;
    c << "{" << endl;
    c << "\"id\": 0," << endl;
    c << "\"map\": [{\"name\": \"MICROSTOMUS\", \"description\": \"Microstomus\", \"color\": \"89  233 206\"}]" << endl;
    c << "}" << endl;
    c << "]" << endl;
    c << "}" << endl;

    try {
        Config cfg("config.json");
        ConfigMaps maps;
        initConfigMaps("config_map.json", maps);

        const Rect bbox(0, 0, 0, 0);
        const unsigned int occluded_pixels = 0;
        const unsigned int frame = 0;
        VOCObject voc;
        voc.setName("Unknown");
        voc.setConfidence(1.0);
        voc.setBox(Rect(0,0,100,100));
        voc.setIndex(1.0);
        EventObject object(voc, occluded_pixels, frame);
        Mat img = cv::imread("data/benthic/f001000.png");
        boost::uuids::uuid u;
        VisualEvent event(u, img, object, cfg, maps);
    } catch(std::exception&) {
        cout << "error" << endl;
        return -1;
    }
}