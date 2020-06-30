#include <iostream>
#include <string>
#include <stdio.h>
#include <nlohmann/json.hpp>
#include "config_class_map.h"

using namespace nlohmann;
using namespace std;

namespace deepsea {

     void from_json(const json& j, ConfigMaps& p) {
        for (auto& el : j["items"].items())
        {
            int r,g,b;
            int id = (int) el.value()["id"] ;
            for (auto& inner_el : el.value()["map"].items()) {
                cout << inner_el.value() << endl;
                p.class_ids[inner_el.value().at("name")] = id;
                p.class_descriptions[id] = inner_el.value().at("description");
                string s = inner_el.value().at("color");
                sscanf(s.c_str(), "%d %d %d", &r, &g, &b);
                p.class_colors[id] = cv::Scalar(r % 256,g % 256,b % 256);
            }
        }
    };

    bool initConfigMaps(string filename, ConfigMaps& cfg) {
        ifstream fin(filename);
        try {
            json j = json::parse(fin);
            from_json(j, cfg);
        } catch(std::exception& e) {
            std::cout << "error" <<  e.what() << std::endl;
            return false;
        }
        return true;
    };

} // namespace deepsea