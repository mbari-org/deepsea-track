#include <fstream>              // ofstream
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>    // json
#include "logger.h"
#include "event_object.h"

using namespace std;
using namespace nlohmann;

namespace deepsea {

// ######################################################################
    Logger::Logger(const Config &cfg, float resize_per, unsigned int start_frame_num, string out_dir)
            : cfg_(cfg),
              resize_per_(resize_per),
              start_frame_num_(start_frame_num),
              out_dir_(out_dir){
    }

// ######################################################################
    Logger::~Logger()
    {
    }

// ######################################################################
    void Logger::save(list<VisualEvent *> events, unsigned int frame_num) {

        list<VisualEvent *>::iterator itve;
        ofstream out_file(out_dir_ + cv::format("f%06d.json", frame_num));
        json j = json::array();

        for (itve = events.begin(); itve != events.end(); ++itve) {

            // only log events that are validm
            if ((*itve)->getState() == VisualEvent::State::VALID) {
                uuids::uuid id = (*itve)->getUUID();
                EventObject vo = (*itve)->getLatestObject();
                json voj;
                EventObject::to_json(voj, vo);
                j.push_back({"visualevent", voj});

            }

        }

        out_file << json::array({"visualevents", j});
    }
}
