#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include "util/config.h"
#include "event_object.h"


using namespace std;
using namespace cv;
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

    const Rect bbox(0, 0, 0, 0);
    const unsigned int occluded_pixels = 0;
    const unsigned int frame = 0;
    VOCObject voc;
    voc.setName("Unknown");
    voc.setConfidence(1.0);
    voc.setBox(Rect(0,0,100,100));
    voc.setIndex(1.0);
    EventObject object(voc, occluded_pixels, frame);
}