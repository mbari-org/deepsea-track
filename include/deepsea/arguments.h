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

#include <iostream>

using namespace std;

namespace deepsea {

    class Arguments {

    public:

        /// \brief Constructor
        Arguments(int argc, char **argv);

        virtual ~Arguments();

        ///
        //" Usage: deepsea-track <path to video> <path to xml> <start frame num> <frame resize width> < frame resize height> <stride(optional)>\n"
        //" examples:\n"
        string getVideoPath();

    private:
        string video_path_;               ///! absolute path to the video file to process
        unsigned int start_frame_num_;    ///! starting frame; used only during startup
        string out_dir_;                    ///! output directory to save all output to
    };
}

