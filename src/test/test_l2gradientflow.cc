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

int edgeThresh = 1;
int edgeThreshScharr=1;

Mat enhanced, frame, frame_resized, gray_resized, blurImage, edge1, edge2, cedge;
const char* window_name0 = "Raw";
const char* window_name1 = "Enhanced";
const char* window_name2 = "Edge map : Canny default (Sobel gradient)";
const char* window_name3 = "Edge map : Canny with custom gradient (Scharr)";

// define a trackbar callback
static void onTrackbar(int, void*)
{
    // show image with the preprocessed frame
//    imshow(window_name0, frame_resized)

    cvtColor(frame_resized, gray_resized, COLOR_BGR2GRAY);

    blur(gray_resized, blurImage, Size(3,3));
    // Run the edge detector on grayscale
    Canny(blurImage, edge1, edgeThresh, edgeThresh*3, 3, true);
    cedge = Scalar::all(0);
    frame_resized.copyTo(cedge, edge1);
    imshow(window_name2, cedge);

    Mat dx,dy;
    Scharr(blurImage,dx,CV_16S,1,0);
    Scharr(blurImage,dy,CV_16S,0,1);
    Canny( dx,dy, edge2, edgeThreshScharr, edgeThreshScharr*3, true );
    cedge = Scalar::all(0);
    frame_resized.copyTo(cedge, edge2);
    imshow(window_name3, cedge);
}

int main( int argc, char** argv ) {
    // show help
    if(argc<2){
        cout<<
            " Usage: test_l2gradientflow <video_name>>\n"
            " examples:\n"
            " test_l2gradientflow $PWD/data/benthic/D0232_03HD_00-02-30.mov $PWD/data/benthic/ \n"
            << endl;
        return 0;
    }

    // set input video
    string video = argv[1];
    VideoCapture cap(video);
    Size scaled_size(Size(
    (int) cap.get(CAP_PROP_FRAME_WIDTH)*0.5,
            (int) cap.get(CAP_PROP_FRAME_HEIGHT)*0.5));
    Preprocess pre(scaled_size, 2, video);

    printf("Start the gradient flow, press ESC to quit.\n");

    // Create a window
    namedWindow(window_name1, 1);
    namedWindow(window_name2, 1);

    // create a toolbar
    createTrackbar("Canny threshold default", window_name1, &edgeThresh, 100, onTrackbar);
    createTrackbar("Canny threshold Scharr", window_name2, &edgeThreshScharr, 400, onTrackbar);

    while(cap.read(frame)) {

        resize(frame, frame_resized, cv::Size(), 0.5, 0.5);

        // enhance
        Mat enhanced = pre.update(frame_resized);
        onTrackbar(0, 0);

        //quit on ESC button
        waitKey();
        if(waitKey(1)==27)break;
    }
}
//#endif