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

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <string>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <nlohmann/json.hpp>
#include <deepsea/util/voc_object.h>
#include <deepsea/util/voc_utils.h>
#include <deepsea/preprocess.h>
#include <deepsea/util/utils.h>
#include <deepsea/visual_event_mgr.h>
#include <deepsea/logger.h>

using namespace cv;
using namespace std;
using namespace deepsea;

int main( int argc, char** argv ) {

    // show help
    if (argc < 2) {
        cout <<
             " Usage: deepsea-track <path to video> <path to xml> <start frame num> <frame resize width> < frame resize height> <stride(optional)>\n"
             " examples:\n"
             " deepsea-track $PWD/data/benthic/D0232_03HD_00-02-30.mov $PWD/data/benthic/ 1 512 512 \n"
             << endl;
        return -1;
    }

    try {
        XMLPlatformUtils::Initialize();
    } catch(...) {
        cout << "error initializing XML reader:" << endl;
        return -1;
    }

    int frame_num = 1;
    int stride = 1;
    list<VOCObject> vocs;
    XercesDOMParser *parser = new XercesDOMParser;
    Mat frame, frame_enhanced, frame_resized;
    unsigned int start_frame_num;
    int resize_height, resize_width;
    string video = argv[1];
    string xml_path = argv[2];
    stringstream ssf;           ///! starting frame
    ssf << argv[3];
    ssf >> start_frame_num;     ///! percent to resize video
    stringstream ssw;
    ssw << argv[4];
    ssw >> resize_width;
    stringstream ssh;
    ssh << argv[4];
    ssh >> resize_height;
    stringstream sss;
    if (argc >= 8) {
        sss << argv[7];
        sss >> stride;          ///! stride parameter
    }

    // Write the configuration.
    cout << "Run configuration: starting frame (" << start_frame_num;
    cout << "), resize (" << resize_width <<  "x" << resize_height << "), stride (" << stride;
    cout << ")" << endl;

    ConfigMaps cfg_map;
    std::string filename = xml_path + "deepsea_class_map.json";
    try {
        initConfigMaps(filename, cfg_map);
    } catch(...) {
        cout << "error loading class maps:" << filename << endl;
        return -1;
    }

    filename = xml_path + "deepsea_cfg.json";
    Config cfg(filename);
    assert(cfg.isInitialized());

    std::cout << cfg.getProgram() << endl;

    VideoCapture cap(video);
    int width  = cap.get(CAP_PROP_FRAME_WIDTH);
    int height = cap.get(CAP_PROP_FRAME_HEIGHT);
    float fps = cap.get(CAP_PROP_FPS);
    float resize_factor_width = float(resize_width) / float(width);
    float resize_factor_height = float(resize_height) / float(height);
    if (start_frame_num > 1) {
        cap.set(CAP_PROP_POS_FRAMES, start_frame_num);
        frame_num = start_frame_num;
    }

    VideoWriter out(xml_path + "results.mp4",
            VideoWriter::fourcc('H','2','6','4'),
            fps, Size(width,height));

    Size scaled_size(Size(resize_width, resize_height));
    Preprocess pre(scaled_size, 3, video);
    Logger log(cfg, start_frame_num, xml_path);
    VisualEventManager manager(cfg, cfg_map);

    cout << "Starting " << cfg.getProgram() << " , press ESC to quit" << endl;


    while(cap.read(frame)) {

        cout << "====================== Processing frame " << frame_num << "====================== " << endl;

        if (frame_num == start_frame_num || (frame_num % stride) == 0) {
            // read xml
            parser->resetDocumentPool();
            string xml_filename = cv::format("%s/f%06d.xml", xml_path.c_str() , frame_num);

            if (Utils::doesPathExist(xml_filename)) {
                parser->parse(xml_filename.c_str());
                getObjectValues(parser, vocs, cfg_map);
            }
        }

        double timer = (double)getTickCount();
        resize(frame, frame_resized, Size(), resize_factor_width, resize_factor_height);

        // enhance
        frame_enhanced = pre.update(frame_resized);

        // rescale and store VOCObjects in VisualObjects
        list<VOCObject>::iterator itvoc;
        list<EventObject> vobs;
        for (itvoc = vocs.begin(); itvoc != vocs.end(); ++itvoc) {
            Rect2d  r = (*itvoc).getBox();
            r.x = int(r.x * resize_factor_width);
            r.y = int(r.y * resize_factor_height);
            r.width = int(r.width * resize_factor_width);
            r.height = int(r.height * resize_factor_height);
            (*itvoc).setBox(r);
            EventObject o((*itvoc), 0, frame_num);
            vobs.push_back(o);
        }

        // run the manager and time it
        manager.run(vobs, frame_enhanced, frame_num);
        fps = getTickFrequency() / (getTickCount() - timer);

        // get list of visual objects
        list<VisualEvent *> events = manager.getEvents(frame_num);

        // log to json
        log.save(events, frame_num, resize_factor_width, resize_factor_height);

        if (cfg.display()) {
            list<VisualEvent *>::iterator itve;
            for (itve = events.begin(); itve != events.end(); ++itve) {

                // only show valid events
                if ((*itve)->getState() != VisualEvent::State::VALID) {
                    continue;
                }

                EventObject evt_obj = (*itve)->getLatestObject();

                // rescale boxes in case tracking on reduced frame size
                Rect2d bbox = evt_obj.getBboxTracker();
                Rect2d bbox_tracker = Utils::rescale(resize_factor_width, resize_factor_height, bbox);

                // descriptive information for the frame overlay
                uuids::uuid id = (*itve)->getUUID();
                Scalar color = cfg_map.class_colors[evt_obj.getClassIndex()];
                string class_name;
                // use class description if available
                if (evt_obj.getClassIndex() > 0)
                    class_name = cfg_map.class_descriptions[evt_obj.getClassIndex()];
                else
                    class_name = evt_obj.getClassName();

                // overlay box and prediction
                string uuid = boost::uuids::to_string(id);
                string description = class_name;//format("%s,%4.2f,%4.2f", class_name.c_str(), vo.bbox_tracker_.x, vo.bbox_tracker_.y);
                //string description = cv::format("%s,%g", class_name.c_str(), vo.surprise_);
                Utils::decorate(frame, bbox_tracker, color, uuid.c_str(), class_name, 1, 0.7);
            }

            string msg = cv::format("FPS: %2.4f frame: %06d", fps, frame_num);
            putText(frame, msg.c_str(), Point(int(0.025 * width), int(0.025 * height)),
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(53, 200, 243), 1);
            imshow("track", frame);
            imshow("enhanced", frame_enhanced);
            out.write(frame);

            if (waitKey(cfg.displayWait()) == 27) // quit on ESC button
                break;
        }

        frame_num +=1;
        vocs.clear();
    }

    cout << "Done!" << endl;
    cap.release();
    out.release();
    destroyAllWindows();
    return 0;
}
