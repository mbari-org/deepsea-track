#include <iostream>
#include <string>
#include <zmq.hpp>
#include <deepsea/util/utils.h>
#include <deepsea/zmq_listener.h>

using namespace std;

namespace deepsea {

// ######################################################################
    ZMQListener::ZMQListener(const string address, const string topic,
                            const float tracker_width,
                            const float tracker_height):
            started_(false),
            valid_(false),
            address_(address),
            topic_(topic),
            context_(1),
            subscriber_(context_, ZMQ_SUB),
            tracker_width_(tracker_width),
            tracker_height_(tracker_height){
        // rudimentary check for populated address/topic; todo: add check for correct protocol
        if (address.length() > 0 and topic.length() > 0)
            valid_ = true;
    }

// ######################################################################
    ZMQListener::~ZMQListener() {
    }

// ######################################################################
    void ZMQListener::init() {

        try {
            const std::chrono::milliseconds timeout{120000000};
            string start_msg = "start";
            subscriber_.connect(address_);
            subscriber_.set(zmq::sockopt::subscribe, topic_);
            this->started_ = true;
        }
        catch (zmq::error_t error) {
            cout << error.what() << endl;
            exit(-1);
        }
    }

// ######################################################################
    void ZMQListener::listen(list<EventObject> &objects, unsigned int target_frame_num) {

        try {
            zmq::message_t msg;
            cout << "Listening for visual events topic " << topic_ << " on " << address_ << " frame " << target_frame_num << "..." << endl;

            while(true) {
                subscriber_.recv(msg, zmq::recv_flags::none);
                string s = string(static_cast<char*>(msg.data()), msg.size());
                float xmin, xmax, ymin, ymax;
                float class_score;
                string class_name;
                if(s == topic_) {
                    zmq::message_t json_msg;
                    subscriber_.recv(json_msg, zmq::recv_flags::none);
                    string json_str = string(static_cast<char*>(json_msg.data()), json_msg.size());
                    nlohmann::json vocs = nlohmann::json::parse(json_str);
                    if (vocs.size() > 0) {
                        cout << vocs << endl;
                        cout << vocs.size() << endl;
                        for (int i=0; i < vocs.size(); i++) {
                            string frame_num = vocs[i]["frame_num"];
                            unsigned int recvd_frame = (unsigned int )std::stoi(frame_num);
                            if (vocs[i].is_object() && target_frame_num == recvd_frame) {
                                string xmn = vocs[i]["xmin"]; xmin = std::stof(xmn);
                                string xmx = vocs[i]["xmax"]; xmax = std::stof(xmx);
                                string ymn = vocs[i]["ymin"]; ymin = std::stof(ymn);
                                string ymx = vocs[i]["ymax"]; ymax = std::stof(ymx);
                                string class_name = vocs[i]["class_name"];
                                string score = vocs[i]["class_score"]; class_score = std::stof(score);
                                // rescale and store in EventObject
                                Rect box = Rect(int(tracker_width_*xmin),
                                                int(tracker_height_*ymin),
                                                int(tracker_width_*(xmax - xmin)),
                                                int(tracker_height_*(ymax - ymin)));
                                VOCObject v(class_name, class_score, box);
                                objects.push_back(EventObject(v, 0, target_frame_num));
                            }
                        }
                    }
                    cout << "Received " << objects.size() << " objects" << " for topic " << topic_ << " on " << address_ << " frame " << target_frame_num << endl;
                    return;
                }
            }
        }
        catch (zmq::error_t error) {
            cout << error.what() << endl;
            exit(-1);
        }
    }


// ######################################################################
    bool ZMQListener::valid() {
        return this->valid_;
    }
}
