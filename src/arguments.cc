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
#include <sys/stat.h>
#include <boost/program_options.hpp>
#include <deepsea/arguments.h>

using namespace std;
namespace po = boost::program_options;

namespace deepsea {

// ######################################################################
    Arguments::Arguments(int argc, char **argv){

        initialized_ = false;
        po::options_description desc("Allowed Arguments");
        desc.add_options()
                ("help", "produce help message")
                ("start_frame_num", po::value<unsigned int>(&start_frame_num_)->default_value(0),
                        "starting frame to process, e.g. --start_frame=0 is the first frame in the video")
                ("resize_width", po::value<unsigned int>(&resize_width_)->default_value(512),
                        "resize width in pixels for running the tracker")
                ("resize_height", po::value<unsigned int>(&resize_height_)->default_value(512),
                        "resize width in pixels for running the tracker")
                ("stride", po::value<unsigned int>(&stride_)->default_value(1),
                        "stride in detections, 1-based. e.g. --stride=5 will skip ip seeding new events to every 5th detection")
                ("video_name", po::value<string>(&video_name_),
                        "name of the video file to process")
                ("out_path", po::value<string>(&out_path_),
                 "absolute path to save all output to")
                ("in_path", po::value<string>(&in_path_),
                        "absolute path to the input artifacts: video.mov, deepsea_class_map.json and deepsea_cfg.jsonfiles")
                ("xml_path", po::value<string>(&xml_path_),
                 "absolute path to directory with voc xml files. If absent, --address and --topic must be set")
                ("address", po::value<string>(&address_),
                        "socket address for the detector output, e.g. tcp://127.0.0.1:6432")
                ("topic", po::value<string>(&topic_),
                        "topic to listen on at address, e.g. VisualEvents ")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        struct stat buffer;

        if (vm.count("help")) {
            cout << desc << "\n";
            return;
        }
        bool failure = this->detection_options(vm);

        if (vm.count("in_path")) {
            if (stat (in_path_.c_str(), &buffer) != 0) {
                cout << "Directory " << in_path_ << " does not exist" << endl;
                failure = true;
            }
            else {
                cout << "Input path was set to " << in_path_ << endl;
            }
        }
        else {
            cout << "Input path must be set with --in_path" << endl;
            failure = true;
        }
        if (vm.count("video_name")) {
            string video_path =  this->in_path_ + "/" + this->video_name_;
            if (stat (video_path.c_str(), &buffer) != 0) {
                cout << video_path << " does not exist";
                failure = true;
            }
            else {
                cout << "Video to process was set to " << video_path << endl;
            }
        } else {
            cout << "Must set the video_name with --video_name and input with --input_path" << endl;
            failure = true;
        }
        if (vm.count("out_path")) {
            if (stat (out_path_.c_str(), &buffer) != 0) {
                cout << "Directory " << out_path_ << " does not exist" << endl;
                failure = true;
            }
            else {
                cout << "Output path to log files was set to " << out_path_ << endl;
            }
        }
        if (!vm.count("start_frame_num")) {
            start_frame_num_ = 0;
        }
        cout << "Start processing frame " << start_frame_num_ << endl;

        if (vm.count("resize_width") && vm.count("resize_height")) {
            cout << "Resizing to " << resize_width_ << "x" << resize_height_ << endl;
        }
        if (vm.count("stride")) {
            cout << "Skipping every " << stride_ << " detection" << endl;
        }
        if (!failure)
            initialized_ = true;
    }

// ######################################################################
    Arguments::~Arguments()
    {
    }

    bool Arguments::detection_options(const boost::program_options::variables_map & vm)
    {
        bool failure = false;
        struct stat buffer;

        if (vm.count("xml_path") && (vm.count("address") || vm.count("topic") ))  {
            throw std::logic_error(std::string("Specify --topic and --address or detection input from --xml_path"));
        }

        if (vm.count("xml_path")) {
            if (stat (xml_path_.c_str(), &buffer) != 0) {
                cout << "Directory " << xml_path_ << " does not exist" << endl;
                failure = true;
            }
            else {
                cout << "Input path with xml detections was set to " << xml_path_ << endl;
            }
        }
        if (vm.count("topic") && vm.count("address")) {
            cout << "Listening for topic " << topic_ << " at " << address_ << endl;
        }
        return failure;

    }

}