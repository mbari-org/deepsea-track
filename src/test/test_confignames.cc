#include "include/mbariviz/util/ConfigClassMap.h"

#include <iostream>
#include <string>

using namespace std;
using namespace avedac;

int main()
{
    string filename = "/Users/dcline/Sandbox/avedac-track-opencv/deepsea_class_map.json";
    try {
        ConfigMaps cfg;
        initConfigMaps(filename, cfg);
        cout << cfg.animal_track_ids["one"] << endl;
        cout << cfg.animal_track_ids["two"] << endl;
    } catch(exception&) {
        cout << "error" << endl;
    }
}