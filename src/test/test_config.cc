#include <string>
#include <iostream>
#include "util/config.h"

using namespace std;
using namespace deepsea;

int main()
{
    ofstream cfg_file;
    cfg_file.open("config.json");
    cfg_file << "{" << endl;
    cfg_file << "\"program\": \"mbariviz\"," << endl;
    cfg_file << "\"tracker\": 0," << endl;
    cfg_file << "\"min_event_frames\": 3," << endl;
    cfg_file << "\"kalman_pnoise\": 0.1," << endl;
    cfg_file << "\"kalman_mnoise\": 10.0" << endl;
    cfg_file << "}" << endl;
    cfg_file.close();

    Config cfg("config.json");
    assert(cfg.isInitialized());
}