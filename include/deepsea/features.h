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

///\file features.h Compute image features used in tracking

#include <map>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

namespace deepsea {

    Mat getInner(const Mat &img, int border);
    Scalar_<double>  computeBorderMean(const Mat &img);
    Scalar_<double> negMean(const Mat &img);
    Scalar_<double> posMean(const Mat &img);
    Mat raisePower(const Mat &img, int n);
    void sobelExtractor(const Mat img, const Rect roi, Mat& feat);
    void localJetsExtractor(const Mat img, const Rect roi, const int scale, vector<double> &jet_features);
}