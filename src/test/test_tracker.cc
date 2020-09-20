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
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <sys/stat.h>
#include "include/deepsea/util/VOCObject.h"
#include "include/deepsea/util/VOCUtils.h"

using namespace std;
using namespace cv;

bool doesPathExist(const std::string &s)
{
    struct stat buffer;
    return (stat (s.c_str(), &buffer) == 0);
}

int main( int argc, char** argv ) {
    // show help
    if (argc < 2) {
        cout <<
             " Usage: test_tracker <video_name> <path to xml> <start frame num> <frame resize ratio>\n"
             " examples:\n"
             " voc_test $PWD/data/benthic/D0232_03HD_00-02-30.mov $PWD/data/benthic/ 1 0.5\n"
             << endl;
        return 0;
    }

    try {
        XMLPlatformUtils::Initialize();
    }
    catch(...) {
        printf("Exception initializing XML reader");
        return -1;
    }
    list<VOCObject> vocs;
    XercesDOMParser *parser = new XercesDOMParser;
    Mat frame;
    Mat frame_resized;
    unsigned int frame_num;
    float resize_ratio;
    // set input video
    string video = argv[1];
    // set path to xml
    string xml_path = argv[2];
    // starting frame
    stringstream ss1;
    ss1 << argv[3];
    ss1 >> frame_num;
    // percent to resize video
    stringstream ss2;
    ss2 << argv[4];
    ss2 >> resize_ratio;

    // these are the animals we want to track
    string animals[3] = {"MITROCOMA", "KRILL","MICROSTOMUS"};
    std::map<string, int> animal_track_ids;
    animal_track_ids["MITROCOMA"] = -1;
    animal_track_ids["KRILL"] = -1;
    animal_track_ids["MICROSTOMUS"] = -1;

    VideoCapture cap(video);
    int frames = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int width  = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);
    Ptr<MultiTracker> multi_tracker = MultiTracker::create();
    cout << "Video has " << frames << " frames of dimensions " << width << ", " << height << ")." <<endl;

    printf("Start the preprocess, press ESC to quit.\n");
    while(cap.read(frame)) {

        // read xml
        parser->resetDocumentPool();
        string xml_filename = format("%s/f%06d.xml", xml_path.c_str() , frame_num);

        if (doesPathExist(xml_filename)) {
            parser->parse(xml_filename.c_str());
            getObjectValues(parser, vocs, width, height);
        }

        cv::resize(frame, frame_resized, cv::Size(), resize_ratio, resize_ratio);

        std::list<VOCObject>::const_iterator itv = vocs.begin();
        const std::vector<Rect2d>& objects = multi_tracker->getObjects();
        unsigned int num_objs = objects.size();

        // go through each voc object and initialize the tracker
        while (itv != vocs.end()) {
            Rect bbox = (*itv).getBox();
            std::string class_name = (*itv).getName();

            // first time we've seen this animal, so let's create a new tracker
            if (animal_track_ids[class_name] == -1) {
                printf("Initializing tracker for new animal %s\n", class_name.c_str());
                const Rect2d bbox_scaled(int(resize_ratio*bbox.x), int(resize_ratio*bbox.y), int(resize_ratio
                *bbox.width), int(resize_ratio*bbox.height));
                multi_tracker->add(TrackerKCF::create(), frame_resized, bbox_scaled);
                animal_track_ids[class_name] = num_objs; // track is is simply the index of the object
            }
            ++itv;
        }

        // run the tracker
        bool ok = multi_tracker->update(frame);

        // show tracked frame
        const std::vector<Rect2d>& tracked_objects = multi_tracker->getObjects();
        std::vector<Rect2d>::const_iterator ito = tracked_objects.begin();
        while (ito != tracked_objects.end()) {
            rectangle(frame_resized, *ito, Scalar( 255, 0, 0 ), 2, 1 );
            ++ito;
        }

        imshow("objects", frame_resized);

        //quit on ESC button
        waitKey();
        if (waitKey(1) == 27)break;

        frame_num +=1;
        vocs.clear();
    }
}