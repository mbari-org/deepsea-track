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
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <list>
#include "preprocess.h"

using namespace std;
using namespace cv;
using namespace deepsea;


int main( int argc, char** argv ) {
    // show help
    if(argc<2){
        cout<<
            " Usage: test_preprocess <video_name>>\n"
            " examples:\n"
            " test_preprocess $PWD/data/benthic/D0232_03HD_00-02-30.mov $PWD/data/benthic/ \n"
            << endl;
        return 0;
    }

    Mat frame;
    Mat frame_resized;

    // set input video
    string video = argv[1];
    VideoCapture cap(video);
    Size scaled_size(Size(
            (int) cap.get(CAP_PROP_FRAME_WIDTH)*0.5,
            (int) cap.get(CAP_PROP_FRAME_HEIGHT)*0.5));
    Preprocess pre(scaled_size, 2, video);

    printf("Start the preprocess, press ESC to quit.\n");

    while(cap.read(frame)) {

        resize(frame, frame_resized, cv::Size(), 0.5, 0.5);

        // enhance
        Mat enhanced = pre.update(frame_resized);

        // show image with the preprocessed frame
        imshow("original", frame_resized);
        imshow("enhanced", enhanced);

        //quit on ESC button
        waitKey();
        if(waitKey(1)==27)break;
    }
}