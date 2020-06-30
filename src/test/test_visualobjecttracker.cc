//
// Created by Danelle Cline on 3/30/20.
//
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <sys/stat.h>
#include "include/mbariviz/util/VOCObject.h"
#include "include/mbariviz/util/VOCUtils.h"
#include "include/mbariviz/Preprocess.h"

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
             " Usage: tracker <video_name> <path to xml> <start frame num> <frame resize ratio>\n"
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
    Mat frame, frame_enhanced, frame_resized;
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
    string animals[7] = {"MITROCOMA", "KRILL","MICROSTOMUS", "Holothuroidea", "Echinoidea", "BATHOCHORDAEUS", "BATHOCHORDAEUS HOUSE"};
    std::map<string, int> animal_track_ids;
    animal_track_ids["MITROCOMA"] = -1;
    animal_track_ids["KRILL"] = -1;
    animal_track_ids["MICROSTOMUS"] = -1;
    animal_track_ids["Holothuroidea"] = -1;
    animal_track_ids["Echinoidea"] = -1;
    animal_track_ids["BATHOCHORDAEUS HOUSE"] = -1;
    animal_track_ids["BATHOCHORDAEUS"] = -1;

    VideoCapture cap(video);
    int width  = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);

    Size scaled_size(Size(    (int) width*resize_ratio, (int) height*resize_ratio));
    Preprocess pre(scaled_size, 2, video);

    Ptr<MultiTracker> multi_tracker = MultiTracker::create();

    printf("Start the preprocess, press ESC to quit.\n");
    VideoCapture cap2(video);
    while(cap2.read(frame)) {

        // read xml
        parser->resetDocumentPool();
        string xml_filename = format("%s/f%06d.xml", xml_path.c_str() , frame_num);

        if (doesPathExist(xml_filename)) {
            parser->parse(xml_filename.c_str());
            getObjectValues(parser, vocs, width, height);
        }

        cv::resize(frame, frame_resized, cv::Size(), resize_ratio, resize_ratio);

        // enhance
        frame_enhanced = pre.update(frame_resized);

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
                const Rect2d bbox_scaled(int(resize_ratio*bbox.x), int(resize_ratio*bbox.y),
                                int(resize_ratio*bbox.width), int(resize_ratio*bbox.height));
                multi_tracker->add(TrackerMedianFlow::create(), frame_enhanced, bbox_scaled);
                animal_track_ids[class_name] = num_objs; // track is is simply the index of the object
            }
            ++itv;
        }

        // run the tracker
        bool ok = multi_tracker->update(frame_enhanced);

        if (ok) {
            string msg = format("Tracking ok frame: %06d", frame_num);
            putText(frame_enhanced, msg.c_str(), Point(20, 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);
            const std::vector<Rect2d>& tracked_objects = multi_tracker->getObjects();
            std::vector<Rect2d>::const_iterator ito = tracked_objects.begin();
            while (ito != tracked_objects.end()) {
                rectangle(frame_enhanced, *ito, Scalar( 255, 0, 0 ), 2, 1 );
                ++ito;
            }
        }
        else
            putText(frame_enhanced, "Tracking failure detected", Point(20,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);

        imshow("objects", frame);
        imshow("track", frame_enhanced);

        //quit on ESC button
        //waitKey();
        if (waitKey(1) == 27)break;

        frame_num +=1;
        vocs.clear();
    }

    frame.release();
    frame_resized.release();
    frame_enhanced.release();
}