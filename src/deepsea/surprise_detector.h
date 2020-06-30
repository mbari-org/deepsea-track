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

///\file  SurpriseImage.h  wrapper class containing surprise detectors based on Poisson model for detecting
/// surprising events in video/

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <vector>
#include "poisson_model.h"

using namespace std;
using namespace cv;

namespace deepsea {
    class SurpriseDetector {
    public:

        SurpriseDetector(const Size &sz);

        SurpriseDetector(double update_factor, const Mat1d &sample);

        void resetUpdateFactor(double factor);

        void update(const SurpriseDetector &other);

        Mat1d surprise(const SurpriseDetector &other);

        Size getSize() const;

        Mat1d getImage() const;

        vector <PoissonModel> getModels() const;

    private:
        vector <PoissonModel> models_;
        Mat1d img_;
        double update_factor_;
    };
}
