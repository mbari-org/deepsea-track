#include <string>
#include <sstream>
#include <iostream>
#include <boost/format.hpp>
#include <nlohmann/json.hpp>
#include <deepsea/util/config.h>
#include <deepsea/version.h>

namespace deepsea {

    void Config::from_json(const nlohmann::json &j) {
        tracker_cfg_.type = j.at("tracker").get<TrackerType>();
        tracker_cfg_.score_threshold = j.at("score_threshold");
        tracker_cfg_.stride = j.at("tracker_stride");
        tracker_cfg_.gamma_enhance = j.at("tracker_gamma_enhance");
        tracker_cfg_.nms_threshold = j.at("nms_threshold");
        j.at("min_event_frames").get_to(tracker_cfg_.min_event_frames);
        j.at("display_wait_msecs").get_to((display_wait_msecs_));
        j.at("tracker_wait_msecs").get_to((tracker_wait_msecs_));
        j.at("display").get_to((display_));
        j.at("create_video").get_to((create_video_));
    }

    void Config::save(nlohmann::json &j) {
        j["program"] = program_info_;
        j["args"] = args_;
        j["tracker"]["score_threshold"] = tracker_cfg_.score_threshold;
        j["tracker"]["nms_threshold"] = tracker_cfg_.nms_threshold;
        j["tracker"]["stride"] = tracker_cfg_.stride;
        j["tracker"]["gamma_enhance"] = tracker_cfg_.gamma_enhance;
        j["tracker"]["min_event_frames"] = tracker_cfg_.min_event_frames;
    }

    void Config::printTracker(TrackerType tracker_type) {

        switch (tracker_type) {
            case TT_KCF:
                std::cout << "KCF tracker" << std::endl;
                init_ = true;
                break;
            case TT_CSRT:
                std::cout << "CSRT tracker" << std::endl;
                init_ = true;
                break;
            case TT_INVALID:
                std::cout << "Invalid tracker!!" << std::endl;
                init_ = true;
                break;
        }
    }
    Config::Config(const std::string filename, const std::vector<std::string> &args) {
        init_ = false;
        display_wait_msecs_ = 250;
        tracker_wait_msecs_ = 100;
        display_ = true;
        args_ = args;
        create_video_ = false;
        std::ifstream fin(filename);
        try {
            nlohmann::json j = nlohmann::json::parse(fin);
            from_json(j);

            std::stringstream ss;
            ss << boost::format("deepsea-track built %1% %2% git: %3%:%4%") % std::string(__DATE__) % std::string(__TIME__) % std::string(COMMIT) % std::string(BRANCH) << std::endl;
            program_info_ = ss.str();
            program_info_.erase(std::remove(program_info_.begin(), program_info_.end(), '\n'), program_info_.end());
            std::cout << program_info_;
            cout << "Tracker : ";  printTracker(tracker_cfg_.type);
            cout << "Tracker stride : " <<  tracker_cfg_.stride << endl;
            cout << "Tracker gamma enhance : " <<  tracker_cfg_.gamma_enhance << endl;
            cout << "Minimum event frames : "<<  tracker_cfg_.min_event_frames  << endl;
        } catch (std::exception &e) {
            std::cout << "ERROR:" <<  filename << ":" << e.what() << std::endl;
            init_ = false;
        }
    }


    // ######################################################################
    Config &Config::operator=(const Config& cfg) {
        this->init_ = cfg.init_;
        this->tracker_cfg_ = cfg.tracker_cfg_;
        this->program_info_ = cfg.program_info_;
        this->display_wait_msecs_ = cfg.display_wait_msecs_;
        this->tracker_wait_msecs_ = cfg.tracker_wait_msecs_;
        this->display_ = cfg.display_;
        this->create_video_ = cfg.create_video_;
        return *this;
    }


}