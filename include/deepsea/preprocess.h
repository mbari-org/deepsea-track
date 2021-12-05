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

///\file preprocess.h Preprocessing images for use later in a \see visual_event.

#include <map>
#include <opencv2/core/utility.hpp>
#include "image_cache.h"

using namespace std;

namespace deepsea {

    class Preprocess {
    public:
        Preprocess(const Size dims, const bool gamma_enhance = false, unsigned int cache_size = 1, string video = NULL);

        virtual ~Preprocess();

        Mat update(const Mat &img);

        Mat getDiffMean(const Mat &img);

    private:
        void updateGammaCurve(const Mat &img, bool init);

        float computeEntropy(const Mat &img);

        Mat computeHist(const Mat &img);

        unsigned int num_processed_;                                //! number of total images processed
        std::map<int, double> pdf_;
        std::map<int, double> cdfw_;
        float entropy_;                                             //! last entropy
        bool gamma_enhance_;                                        //! true if applying gamma enhancement
        ImageCache<Mat3f> cache_;                                   //! cache to use when computing mean
    };
}