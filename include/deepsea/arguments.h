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
//!\file options.h  Manages parsing the command line

#include <boost/program_options.hpp>
#include <iostream>

using namespace std;

namespace deepsea {

    class Arguments {

    public:

        /// \brief Constructor
        Arguments(int argc, char **argv);

        bool detection_options(const boost::program_options::variables_map & vm);

        inline bool hasXml() {
            if (xml_path_.length() > 0)
                return true;
            return false;
        };

        virtual ~Arguments();

        bool initialized_;

        string xml_path_;                ///! absolute path to directory with voc xml files
        string out_path_;                ///! absolute path to save any output artifacts
        unsigned int start_frame_num_;   ///! starting frame; used only during startup
        string video_path_;              ///! absolute path to the video file
        string cfg_path_;                ///! absolute path to the configuration files: deepsea_class_map.json and deepsea_cfg.json
        unsigned int tracker_width_;     ///! width in pixels to resize the video before running the tracker
        unsigned int tracker_height_;    ///! height in pixels to resize the video before running the tracker
        unsigned int out_width_;         ///! width in pixels the output should be scaled to
        unsigned int out_height_;        ///! height in pixels the output should be scaled to
        unsigned int stride_;            ///! stride in detections, 1-based. e.g. --stride=5 will skip seeding new events to every 5th detection
        string topic_;                   ///! topic to listen on at address, e.g. VisualEvents
        string address_;                 ///! socket address for the detector output, e.g. tcp://127.0.0.1:6432
    };

}

