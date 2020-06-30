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

//#ifndef PREPROCESS_C_DEFINED
//#define PREPROCESS_C_DEFINED

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <iostream>
#include <sys/stat.h>
#include "include/mbariviz/util/VOCObject.h"
#include "include/mbariviz/util/VOCUtils.h"

using namespace std;
using namespace cv;
using namespace xercesc;

bool doesPathExist(const std::string &s)
{
    struct stat buffer;
    return (stat (s.c_str(), &buffer) == 0);
}

int main( int argc, char** argv ) {
    // show help
    if(argc<2){
        cout<<
            " Usage: tracker <video_name> <path to xml>\n"
            " examples:\n"
            " voc_test $PWD/data/benthic/D0232_03HD_00-02-30.mov $PWD/data/benthic/\n"
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

    // set input video
    string video = argv[1];
    // set path to xml
    string xml_path = argv[2];

    VideoCapture cap(video);
    int width  = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);

    printf("Start the preprocess, press ESC to quit.\n");
    unsigned int frame_num = 1;
    unsigned int max_frames = 6;
    for ( ;; ) {
        // read xml
        parser->resetDocumentPool();
        string xml_filename = format("%s/f%06d.xml", xml_path.c_str() , frame_num);

        if (doesPathExist(xml_filename)) {
            parser->parse(xml_filename.c_str());
            getObjectValues(parser, vocs, width, height);
        }

        // get frame from the video
        cap >> frame;

        imshow("original image", frame);

        //go through each object
        std::list<VOCObject>::const_iterator iter = vocs.begin();
        while (iter != vocs.end()) {
            Rect bbox = (*iter).getBox();
            std::string name = (*iter).getName();
            float probability = (*iter).getConfidence();
            rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
            putText(frame, name, Point(bbox.x,bbox.y), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
            ++iter;
        }
        imshow("objects", frame);

        //quit on ESC button
        waitKey();
        if(waitKey(1)==27)break;
        frame_num += 1;
        if (frame_num > max_frames)
            break;

        vocs.clear();
    }
}
//#endif