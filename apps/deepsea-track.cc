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
#include <thread>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <nlohmann/json.hpp>
#include <deepsea/util/voc_object.h>
#include <deepsea/util/voc_utils.h>
#include <deepsea/preprocess.h>
#include <deepsea/util/utils.h>
#include <deepsea/visual_event_mgr.h>
#include <deepsea/logger.h>
#include <deepsea/zmq_listener.h>
#include <deepsea/arguments.h>

using namespace boost;
using namespace cv;
using namespace std;
using namespace deepsea;

int main( int argc, char** argv ) {

    //////////////////////////////////////////////////////////
    // parse arguments
    Arguments args(argc, argv);
    if (!args.initialized_)
        return -1;

    //////////////////////////////////////////////////////////
    // initialize classes and variables
    int frame_num;
    Mat frame, frame_enhanced, frame_resized;
    ConfigMaps cfg_map;
    VideoCapture cap(args.video_path_);
    int frame_width  = cap.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    float fps = cap.get(CAP_PROP_FPS);
    int zmq_delay_microsecs = 5*1e6; // 5 seconds
    float tracker_width = float(args.tracker_width_) ;
    float tracker_height = float(args.tracker_height_) ;
    float resize_frame_factor_width = float(frame_width) / float(args.tracker_width_) ;
    float resize_frame_factor_height = float(frame_height) / float(args.tracker_height_) ;
    float resize_out_factor_width = float(args.out_width_) / float(args.tracker_width_) ;
    float resize_out_factor_height = float(args.out_height_) / float(args.tracker_height_) ;
    if (args.start_frame_num_ > 1)
        cap.set(CAP_PROP_POS_FRAMES, args.start_frame_num_);
    frame_num = args.start_frame_num_;
    stringstream ss;
    ss << boost::format("%s/%s_results.mp4") % args.out_path_ % filesystem::path(args.video_path_).stem().c_str();
    VideoWriter out(ss.str(),
                    VideoWriter::fourcc('H', '2', '6', '4'),
                    fps, Size(frame_width, frame_height));
    Size scaled_size(Size(tracker_width, tracker_height));
    Preprocess pre(scaled_size, 3, args.video_path_);

    //////////////////////////////////////////////////////////
    // get configuration
    cout << "Parsing configuration " << args.cfg_path_ << "deepsea_class_map.json" << endl;
    assert(initConfigMaps(args.cfg_path_ + "deepsea_class_map.json", cfg_map));
    cout << "Parsing configuration" << args.cfg_path_ << "deepsea_cfg.json" << endl;
    Config cfg(args.cfg_path_ + "deepsea_cfg.json");
    assert(cfg.isInitialized());
    Logger log(cfg, frame_num, args.out_path_);
    VisualEventManager manager(cfg, cfg_map);

    //////////////////////////////////////////////////////////
    // if loading from precomputed detections in xml files, initialize the parser
    XercesDOMParser *parser = NULL;
    if (args.xml_path_.length() > 0) {
        try {
            XMLPlatformUtils::Initialize();
        } catch (...) {
            cout << "error initializing XML reader:" << endl;
            return -1;
        }
        parser = new XercesDOMParser;
    }
    //////////////////////////////////////////////////////////
    // if loading detections over zmq, launch thread as daemon to listen for events
    ZMQListener zmq(args.address_, args.topic_, tracker_width, tracker_height);
    thread zmqThread = thread();
    if (zmq.initialized()) {
        thread zmqThread(&ZMQListener::listen, &zmq);
        zmqThread.detach();
    }

    //////////////////////////////////////////////////////////
    // begin processing
    cout << "Starting " << cfg.getProgram() << " , press ESC to quit" << endl;

    while(cap.read(frame)) {

        cout << "====================== Processing frame " << frame_num << "====================== " << endl;

        // if loading detections over zmq, wait for start
        if (zmq.initialized())
            while (!zmq.started()) {
                cout << "waiting for zmq messages to  start" << endl;
                usleep(zmq_delay_microsecs);
            }

        list<EventObject> event_objs;
        list<VOCObject> voc_objs;
        int voc_width, voc_height;
        if (frame_num == args.start_frame_num_ || (frame_num % args.stride_) == 0) {

            // if have voc formatted xml, read detections from files
            if (args.hasXml()) {
                parser->resetDocumentPool();
                string xml_filename = cv::format("%s/f%06d.xml", args.xml_path_.c_str() , frame_num);

                if (Utils::doesPathExist(xml_filename)) {
                    parser->parse(xml_filename.c_str());
                    getObjectValues(parser, voc_objs, cfg_map, voc_width, voc_height);
                    float width_factor = float(tracker_width / voc_width);
                    float height_factor = float(tracker_height / voc_height);
                    // rescale and store VOCObjects in VisualObjects
                    list<VOCObject>::iterator itvoc;
                    for (itvoc = voc_objs.begin(); itvoc != voc_objs.end(); ++itvoc) {
                        Rect2d  r = (*itvoc).getBox();
                        r.x = int(r.x * width_factor);
                        r.y = int(r.y * height_factor);
                        r.width = int(r.width * width_factor);
                        r.height = int(r.height * height_factor);
                        (*itvoc).setBox(r);
                        EventObject o((*itvoc), 0, frame_num);
                        event_objs.push_back(o);
                    }
                }
            }
            else { // otherwise read detections sent over zmq
                event_objs = zmq.getObjects(frame_num);
            }
        }

        double timer = (double)getTickCount();
        resize(frame, frame_resized, Size(), 1. / resize_frame_factor_width, 1. / resize_frame_factor_height);

        // enhance
        frame_enhanced = pre.update(frame_resized);

        // run the manager and time it
        manager.run(event_objs, frame_enhanced, frame_num);
        fps = getTickFrequency() / (getTickCount() - timer);

        // get list of visual objects
        list<VisualEvent *> events = manager.getEvents(frame_num);

        // log to json
        log.save(events, frame_num, resize_out_factor_width, resize_out_factor_height);

        if (cfg.display() || cfg.createVideo()) {
            list<VisualEvent *>::iterator itve;
            for (itve = events.begin(); itve != events.end(); ++itve) {

                // only show valid events
                if ((*itve)->getState() != VisualEvent::State::VALID)
                    continue;

                EventObject evt_obj = (*itve)->getLatestObject();

                // rescale boxes if tracking on reduced frame size
                Rect2d bbox = evt_obj.getBboxTracker();
                Rect2d bbox_tracker = Utils::rescale(resize_frame_factor_width, resize_frame_factor_height, bbox);

                // descriptive information for the frame overlay
                uuids::uuid id = (*itve)->getUUID();
                string uuid = boost::uuids::to_string(id);
                string delimiter = "-";
                string id_str = uuid.c_str();
                string short_uuid = id_str.substr(0, 7);
                Scalar color = cfg_map.class_colors[evt_obj.getClassIndex()];
                string class_name;
                // use class description if available
                if (evt_obj.getClassIndex() > 0)
                    class_name = cfg_map.class_descriptions[evt_obj.getClassIndex()];
                else
                    class_name = evt_obj.getClassName();

                // overlay box and prediction
                string description = class_name;//format("%s,%4.2f,%4.2f", class_name.c_str(), vo.bbox_tracker_.x, vo.bbox_tracker_.y);
                //string description = cv::format("%s,%g", class_name.c_str(), vo.surprise_);
                Utils::decorate(frame, bbox_tracker, color, short_uuid, class_name, 1, 0.7);
            }

            string msg = cv::format("FPS: %2.4f frame: %06d", fps, frame_num);
            putText(frame, msg.c_str(), Point(int(0.025 * frame_width), int(0.025 * frame_height)),
                    FONT_HERSHEY_SIMPLEX, 0.5, Scalar(53, 200, 243), 1);
            if (cfg.display()) {
                imshow("track", frame);
                imshow("enhanced", frame_enhanced);
                if (waitKey(cfg.displayWait()) == 27) // quit on ESC button
                    break;
            }

            if (cfg.createVideo())
                out.write(frame);

        }

        frame_num +=1;
        voc_objs.clear();

        // only pause if current frame is beyond the frame from the last message
        if (cfg.trackerWait() > 0 && frame_num > zmq.lastFrameNum()) {
            cout << "====================== Pausing tracker " << cfg.trackerWait() << " msecs ====================== " << endl;
            usleep(cfg.trackerWait() * 1000);
        }

        if (zmq.initialized())
            zmq.cleanUp(frame_num);
    }

    cout << "Done!" << endl;
    cap.release();
    out.release();
    destroyAllWindows();
    assert(!zmqThread.joinable());
    return 0;
}
