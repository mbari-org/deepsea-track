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
            stopped_(false),
            started_(false),
            initialized_(false),
            address_(address),
            topic_(topic),
            last_frame_num_(-1),
            tracker_width_(tracker_width),
            tracker_height_(tracker_height){
        // rudimentary check for populated address/topic; todo: add check for correct protocol
        if (address.length() > 0 and topic.length() > 0)
            initialized_ = true;
    }

// ######################################################################
    ZMQListener::~ZMQListener() {
        objects_.clear();
    }

// ######################################################################
    list<EventObject> ZMQListener::getObjects(unsigned int frame_num) const {
        list<EventObject> result;
        list<EventObject>::const_iterator evt;
        for (evt = objects_.begin(); evt != objects_.end(); ++evt)
            if (evt->getFrameNum() == frame_num)
                result.push_back(*evt);

        return result;
    }

// ######################################################################
    void ZMQListener::listen() {

        try {
            zmq::context_t context(1);
            zmq::socket_t subscriber(context, ZMQ_SUB);
            const std::chrono::milliseconds timeout{120000};
            string start_msg = "start";
            zmq::message_t msg;
            subscriber.connect(address_);
            subscriber.set(zmq::sockopt::subscribe, topic_);
            zmq::pollitem_t item[0];
            item[0].socket = subscriber;
            item[0].events = ZMQ_POLLIN;
            int rc = zmq::poll(item, 1, timeout); // poll for timeout period only
            assert(rc == 1); //todo put a meaningful message here if does timeout. rc=1 means it returned one item
            this->started_ = true;
            cout << "Listening for visual events topic " << topic_ << " on " << address_ << endl;
            while(!stopped_) {
                auto res = subscriber.recv(msg, zmq::recv_flags::none);
                string s = string(static_cast<char*>(msg.data()), msg.size());
                float xmin, xmax, ymin, ymax;
                float class_score;
                string class_name;
                if(s == topic_) {
                    zmq::message_t json_msg;
                    subscriber.recv(json_msg, zmq::recv_flags::none);
                    string json_str = string(static_cast<char*>(json_msg.data()), json_msg.size());
                    nlohmann::json vocs = nlohmann::json::parse(json_str);
                    if (vocs.size() > 0) {
                        cout << vocs << endl;
                        cout << vocs.size() << endl;
                        for (int i=0; i < vocs.size(); i++) {
                            if (vocs[i].is_object()) {
                                string xmn = vocs[i]["xmin"]; xmin = std::stof(xmn);
                                string xmx = vocs[i]["xmax"]; xmax = std::stof(xmx);
                                string ymn = vocs[i]["ymin"]; ymin = std::stof(ymn);
                                string ymx = vocs[i]["ymax"]; ymax = std::stof(ymx);
                                string class_name = vocs[i]["class_name"];
                                string score = vocs[i]["class_score"]; class_score = std::stof(score);
                                string frame_num = vocs[i]["frame_num"];
                                this->last_frame_num_ = std::stoi(frame_num);
                                // rescale and store in EventObject
                                Rect box = Rect(int(tracker_width_*xmin),
                                        int(tracker_height_*ymin),
                                        int(tracker_width_*(xmax - xmin)),
                                        int(tracker_height_*(ymax - ymin)));
                                VOCObject v(class_name, class_score, box);
                                objects_.push_back(EventObject(v, 0, this->last_frame_num_));
                            }
                        }
                    }
                }
            }
            // cease any blocking operations in progress
            context.shutdown();
            // shutdown
            context.close();
        }
        catch (zmq::error_t error) {
            cout << error.what() << endl;
            exit(-1);
        }
    }

// ######################################################################
    void ZMQListener::cleanUp(const unsigned int max_frame) {
        list<EventObject>::iterator e = objects_.begin();

        while (e != objects_.end()) {
            list<EventObject>::iterator next = e;
            ++next;
            if (e->getFrameNum() < max_frame) {
                objects_.erase(e);
            }
        e = next;
        } // end for loop over events
    }

// ######################################################################
    void ZMQListener::stop() {
        stopped_ = true;
    }

// ######################################################################
    bool ZMQListener::started() {
        return this->started_;
    }

// ######################################################################
    bool ZMQListener::initialized() {
        return this->initialized_;
    }

// ######################################################################
    int ZMQListener::lastFrameNum() {
        return this->last_frame_num_;
    }
}
