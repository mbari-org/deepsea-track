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
//!\file Arguments.h  Manages parsing the command line Arguments

#include <iostream>
#include <boost/program_options.hpp>
#include <deepsea/arguments.h>

using namespace std;
namespace po = boost::program_options;

namespace deepsea {

// ######################################################################
    Arguments::Arguments(int argc, char **argv){

        po::options_description desc("Allowed Arguments");
        desc.add_options()
                ("help", "produce help message")
                ("start_frame_num", po::value<unsigned int>(&start_frame_num_)->default_value(1), "starting frame to process, 1-based. e.g. --start_frame=1 is the first frame in the video")
                ("video_path", po::value<string>(&video_path_), "absolute path to the video file to process")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return;
        }

        //<path to video> <path to xml> <start frame num> <frame resize width> < frame resize height> <stride(optional)>
        if (vm.count("video_path")) {
            cout << "Video to process was set to " << video_path_ << endl;
            //TODO: check video_path existance
        } else {
            cout << "Must set the video_path.\n";
            cout << "Must set the video_path with --video_path";
            return;
        }

        if (vm.count("start_frame_num")) {
            cout << "Start processing frame " << start_frame_num_ << endl;
        }
    }

// ######################################################################
    Arguments::~Arguments()
    {
    }

// ######################################################################
    string Arguments::getVideoPath() {
        return video_path_;
    }
}