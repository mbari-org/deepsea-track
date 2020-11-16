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

///\file  surprise_map.h computes surprise map; used for detecting surprising events in video for visualization
/// and training*/

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include "surprise_detector.h"
#include "event_object.h"

using namespace std;

namespace deepsea {

    class SurpriseMap {
        public:

        //// \brief Default constructor
        SurpriseMap();

        /// \brief Constructor. Creates an detector for every pixel in an image
        /// \param queue_length length in frames of the queue of detectors
        /// \param sz size of the image
        SurpriseMap(const unsigned int queue_length, const Size& sz);

        /// \brief Computes the surprise map
        /// \param image
        /// \return surprise map
        Mat1d surprise(const SurpriseDetector &image);

        /// \brief Convenience method to access detectors.
        /// \param index
        /// \return
        const SurpriseDetector &getSurpriseDetector(const uint index) const;

        private:
            vector <SurpriseDetector> detectors_;
    };
}