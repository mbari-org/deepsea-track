#pragma once
/*
 * Copyright 2021 MBARI
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

///\file  Custom override of trackers for speedup*/

#include <opencv2/tracking.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/cvstd.hpp>
#include "util/config.h"

using namespace cv;
using namespace std;

namespace deepsea {

    class FastTracker : public Tracker
    {
    public:
        static Ptr<Tracker> create(const Config::TrackerConfig config, const float occluded, const Rect& boundingBox);

        FastTracker(const Ptr<Tracker>& tracker, const int stride);

        ~FastTracker() override;

        void init(InputArray image, const Rect& boundingBox) override;

        bool update(InputArray image, Rect& boundingBox) override;

    private:
        const Ptr<Tracker> tracker_;
        int frame_cnt_;
        bool res_;
        int stride_;
        Rect box_;
    };

}