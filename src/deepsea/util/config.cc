#include <string>
#include <sstream>
#include <iostream>
#include <boost/format.hpp>
#include <nlohmann/json.hpp>
#include "config.h"
#include "version.h"

namespace deepsea {

    void Config::from_json(const nlohmann::json &j) {
        tracker_cfg_.type1 = j.at("tracker1").get<TrackerType>();
        tracker_cfg_.type2 = j.at("tracker2").get<TrackerType>();
        j.at("min_event_frames").get_to(tracker_cfg_.min_event_frames);
        j.at("display_wait_msecs").get_to((display_wait_msecs_));
        j.at("display").get_to((display_));
    }

    void Config::to_json(nlohmann::json &j, const Config &p) {
        j = nlohmann::json{{"program",              program_info_}};
    }

    void Config::printTracker(TrackerType tracker_type) {

        switch (tracker_type) {
            case TT_KCF:
                std::cout << "KCF tracker" << std::endl;
                init_ = true;
                break;
            case TT_TLD:
                std::cout << "TLD tracker" << std::endl;
                init_ = true;
                break;
            case TT_MOSSE:
                std::cout << "MOSSE tracker" << std::endl;
                init_ = true;
                break;
            case TT_MEDIANFLOW:
                std::cout << "MEDIANFLOW tracker" << std::endl;
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
    Config::Config(std::string filename) {
        init_ = false;
        display_wait_msecs_ = 250;
        display_ = true;
        std::ifstream fin(filename);
        try {
            nlohmann::json j = nlohmann::json::parse(fin);
            from_json(j);

            std::stringstream ss;
            ss << boost::format("deepsea-track built %1% %2% git: %3%:%4%") % std::string(__DATE__) % std::string(__TIME__) % std::string(COMMIT) % std::string(BRANCH) << std::endl;
            program_info_ = ss.str();
            program_info_.erase(std::remove(program_info_.begin(), program_info_.end(), '\n'), program_info_.end());
            std::cout << program_info_;
            cout << "Tracker 1: ";  printTracker(tracker_cfg_.type1);
            cout << "Tracker 2: ";  printTracker(tracker_cfg_.type2);
            printf("Minimum event frames:%d\n", tracker_cfg_.min_event_frames);
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
        this->display_ = cfg.display_;
        return *this;
    }


}