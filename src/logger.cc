#include <fstream>              // ofstream
#include <opencv2/opencv.hpp>
#include <nlohmann/json.hpp>    // json
#include <deepsea/logger.h>
#include <deepsea/event_object.h>

using namespace std;
using namespace nlohmann;

namespace deepsea {

// ######################################################################
    Logger::Logger(Config cfg, string out_dir)
            : out_dir_(out_dir){
        ofstream out_file(out_dir_ + "args.json");
        json j = json();
        cfg.save(j);
        out_file << j;
    }

// ######################################################################
    Logger::~Logger()
    {
    }

// ######################################################################
    void Logger::save(list<VisualEvent *> events, unsigned int frame_num,
            float resize_factor_width, float resize_factor_height) {

        list<VisualEvent *>::iterator itve;
        ofstream out_file(out_dir_ + cv::format("f%06d.json", frame_num));
        json j = json::array();

        for (itve = events.begin(); itve != events.end(); ++itve) {

            // only log events that are valid
            if ((*itve)->getState() == VisualEvent::State::VALID) {
                EventObject vo = (*itve)->getLatestObject();
                json voj;
                EventObject::to_json(voj, vo, resize_factor_width, resize_factor_height);
                j.push_back({"visualevent", voj});

            }

        }

        out_file << json::array({"visualevents", j});
    }
}
