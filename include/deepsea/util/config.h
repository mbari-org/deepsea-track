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

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>  //json

using namespace std;
using namespace nlohmann;

namespace deepsea {

    class Config {

    public:

        Config(string filename);

        Config() { init_ = false;};

        // example enum type declaration
        enum TrackerType {
            TT_MEDIANFLOW,
            TT_KCF,
            TT_TLD,
            TT_MOSSE,
            TT_CSRT,
            TT_INVALID = -1,
        };

        // map TrackerType values to JSON as strings
        NLOHMANN_JSON_SERIALIZE_ENUM(TrackerType, {
            { TT_INVALID, nullptr },
            { TT_MEDIANFLOW, "MEDIANFLOW" },
            { TT_KCF, "KCF" },
            { TT_TLD, "TLD" },
            { TT_MOSSE, "MOSSE" },
            { TT_CSRT, "CSRT" },
        });

        struct TrackerConfig {
            int min_event_frames;
            TrackerType type1;
            TrackerType type2;
        };

        bool isInitialized() const { return init_; }

        bool display() const { return display_; }

        int displayWait() const { return display_wait_msecs_; }

        int trackerWait() const { return tracker_wait_msecs_; }

        TrackerConfig getTracker() const { return tracker_cfg_; }

        string getProgram() const { return program_info_; }

        //!copy operator
        Config & operator=(const Config& object);

    private:

        void printTracker(TrackerType tracker_type);

        void to_json(json &j, const Config &p);

        void from_json(const json &j);

        bool init_;
        int display_wait_msecs_;            //! Display wait time in milliseconds
        int tracker_wait_msecs_;            //! Tracker wait time in microseconds
        bool display_;                      //! True if displaying output during processing
        string program_info_;
        TrackerConfig tracker_cfg_;
    };
}