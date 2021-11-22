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
//!\file Logger.h  Manages exporting data to various formats

#include "event_object.h"
#include "image_cache.h"
#include "util/config.h"
#include "util/config_class_map.h"
#include "visual_event.h"

using namespace std;

namespace deepsea {

    class Logger {

    public:

        /// \brief Constructor
        /// \param cfg system configuration
        /// \param out_dir output directory to log files to
        Logger(Config cfg, string out_dir);

        virtual ~Logger();

        /// Log all the \see visual_event VisualEvents to JSON files
        /// \param events  list of events
        /// \param frame_num frame number to associate to this collection of event
        /// \param resize_factor_width factor between 0-1 to rescale width
        /// \param resize_factor_height factor between 0-1 to rescale height
        void save(list<VisualEvent *> events, unsigned int frame_num,
                float resize_factor_width=1.,
                float resize_factor_height=1.);

    private:
        string out_dir_;                ///! output directory to log all files to
     };

}
