#include <iostream>
#include <algorithm>                // std::min
#include <boost/uuid/uuid_io.hpp>   // uuids::uuid
#include <opencv2/dnn.hpp>
#include <opencv2/core/ocl.hpp>
#include <deepsea/util/utils.h>
#include <deepsea/visual_event_mgr.h>

using namespace std;
using namespace dnn;

namespace deepsea {

// ######################################################################
    VisualEventManager::VisualEventManager(const Config &cfg, ConfigMaps &cfg_map) {
        cfg_maps_ = cfg_map;
        cfg_ = cfg;
    }

// ######################################################################
    VisualEventManager::~VisualEventManager() {
        events_.clear();
    }

// ######################################################################
    list<VisualEvent *> VisualEventManager::getEvents(unsigned int frame_num) const {
        list<VisualEvent *> result;
        list<VisualEvent *>::const_iterator evt;
        for (evt = events_.begin(); evt != events_.end(); ++evt)
            if ((*evt)->frameInRange(frame_num))
                result.push_back(*evt);

        return result;
    }

// ######################################################################
    list<EventObject> VisualEventManager::getObjects(unsigned int frame_num) const {
        list<EventObject> result;
        list<VisualEvent *>::const_iterator evt;
        for (evt = events_.begin(); evt != events_.end(); ++evt)
            if ((*evt)->frameInRange(frame_num)) {
                result.push_back((*evt)->getLatestObject());
            }

        return result;
    }

// ######################################################################
    list<EventObject> VisualEventManager::runNMS(const list<EventObject> &vobs) {
        vector<int> class_ids;
        vector<float> scores;
        vector<Rect2d> boxes;
        list<EventObject> list_vob;
        vector<int> indices;
        vector<EventObject> vector_vob;

        list<EventObject>::const_iterator itvoc;
        for (itvoc = vobs.begin(); itvoc != vobs.end(); ++itvoc) {
            boxes.push_back((*itvoc).getBboxTracker());
            scores.push_back((*itvoc).getConfidence());
            vector_vob.push_back(*itvoc);
        }

        NMSBoxes(boxes, scores, cfg_.getTracker().score_threshold, cfg_.getTracker().nms_threshold, indices);

        for (size_t i = 0; i < indices.size(); ++i) {
            int idx = indices[i];
            list_vob.push_back(vector_vob[idx]);
        }

        return list_vob;
    }

// ######################################################################
    void VisualEventManager::run(list<EventObject> &evt_objs, const Mat &img, const unsigned int frame_num) {
        VisualEvent *target;
        int sz = evt_objs.size();
        list<EventObject> last = getObjects(frame_num - 1);
        last.insert(last.end(), evt_objs.begin(), evt_objs.end());

        // runNMS on detections to filter overlapping events
        list<EventObject> filtered = runNMS(evt_objs);

        // seed new events with objects that don't intersect with any existing visual events
        list<EventObject>::const_iterator evt_obj;
        for (evt_obj = filtered.begin(); evt_obj != filtered.end(); ++evt_obj) {
            double cc = 0;
            target = findEvent(img, *evt_obj, cc);
            // if cannot find event this object belongs to, create a new event
            if (target == NULL) {
                const uuids::uuid id = (*evt_obj).getUuid();
                printf("**** Adding new event **** %s\n", boost::uuids::to_string(id).c_str());
                addVisualEvent(img, frame_num, *evt_obj);
            } else
                target->updatePrediction(img, *evt_obj, frame_num);
        }

        // update all events
        sz = events_.size();
        cout << frame_num << ":Number of VisualEvents " << sz << endl;
        list<VisualEvent *>::const_iterator itve;
        for (itve = events_.begin(); itve != events_.end(); ++itve)
            (*itve)->updatePrediction(img, frame_num);

        // find largest occlusion with any given event and update the event occlusions
        for (itve = events_.begin(); itve != events_.end(); ++itve) {
            double occlusion = findLargestIntersection((*itve)->getLatestObject());
            if (occlusion > 0.)
                (*itve)->setOcclusion((float) occlusion);
        }
        cleanUp();
    }

// ######################################################################
    void VisualEventManager::addVisualEvent(const Mat &img, const unsigned int frame_num, const EventObject &vob) {
        uuids::uuid uuid1 = random_generator_();
        VisualEvent *evt = new VisualEvent(uuid1, img, vob, cfg_, cfg_maps_);
        evt->updatePrediction(img, vob, frame_num);
        events_.push_back(evt);
    }

// ######################################################################
    double VisualEventManager::findLargestIntersection(const EventObject &evt_obj) {
        double max_iou = 0.;
        list<VisualEvent *>::const_iterator itve;
        Rect2d r1 = evt_obj.getBboxTracker();
        for (itve = events_.begin(); itve != events_.end(); ++itve) {
            EventObject latest_evt_obj = (*itve)->getLatestObject();
            if (evt_obj.getUuid() != latest_evt_obj.getUuid()) {
                Rect2d r2 = latest_evt_obj.getBboxTracker();
                double iou = Utils::iou(r1, r2);
                if (iou > max_iou) {
                    max_iou = iou;
                }
            }
        }
        return max_iou;
    }

    // ######################################################################
    VisualEvent *VisualEventManager::findEvent(const Mat &img, const EventObject &evt_obj, double &cc) {
        list<VisualEvent *>::const_iterator itve;
        double max_iou = 0.;
        double min_cost = numeric_limits<double>::max();
        VisualEvent *target = NULL;

        for (itve = events_.begin(); itve != events_.end(); ++itve) {
            EventObject latest_evt_obj = (*itve)->getLatestObject();
            Rect2d r1 = latest_evt_obj.getBboxTracker();
            Rect2d r2 = evt_obj.getBboxTracker();
            double iou = Utils::iou(r1, r2);
            float cost1 = sqrt(pow((double) (r1.x - r2.x), 2.0) + pow((double) (r1.y - r2.y), 2.0));
            float cost2 = sqrt(pow((double) ((r1.x + r1.width) - (r2.y + r2.height)), 2.0) +
                               pow((double) ((r1.x + r1.width) - (r2.y + r2.height)), 2.0));
            float cost = cost1 + cost2;

            if (iou > 0.f && iou > max_iou && cost < min_cost) {
                target = (*itve);
                max_iou = iou;
                min_cost = cost;
                cout << "*Event " << (*itve)->getUUID() << " cost " << min_cost << endl;
            }
        }
        return target;
    }

// ######################################################################
    void VisualEventManager::cleanUp() {
        list<VisualEvent *>::iterator e = events_.begin();

        while (e != events_.end()) {
            list<VisualEvent *>::iterator next = e;
            ++next;

            switch ((*e)->getState()) {
                case (VisualEvent::CLOSED):
                    cout << "*Erasing event " << (*e)->getUUID() << " at " << (*e)->getLatestObject().getBboxTracker()
                         << endl;
                    delete *e;
                    events_.erase(e);
                    break;
                case (VisualEvent::OPEN):
                    //TODO; put check in for max duration
                    break;
                default:
                    break;
            }

            e = next;
        } // end for loop over events
    }
}
