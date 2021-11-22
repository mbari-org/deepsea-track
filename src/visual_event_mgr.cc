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

        NMSBoxes(boxes, scores, cfg_.getTrackerCfg().score_threshold, cfg_.getTrackerCfg().nms_threshold, indices);

        for (size_t i = 0; i < indices.size(); ++i) {
            int idx = indices[i];
            list_vob.push_back(vector_vob[idx]);
        }

        return list_vob;
    }

// ######################################################################
    void VisualEventManager::run(list<EventObject> &evt_objs, const Mat &img, const Mat &bin_img, const unsigned int frame_num) {
        list<EventObject>::const_iterator itsobjs;
        list<EventObject>::iterator itfobjs, best;
        list<VisualEvent *>::const_iterator itve;
        int sz;
        double iou;
        best = evt_objs.begin();

        // runNMS on detections to filter overlapping events
        list<EventObject> fobjs = runNMS(evt_objs);

        // seed new events with objects that don't match or intersect with any existing visual events
        for (itve = events_.begin(); itve != events_.end(); ++itve) {
            if (fobjs.empty()) break;
            EventObject o = (*itve)->getLatestObject();
            Rect2d r1 = o.getBboxTracker();
            VisualEvent *ves_target = NULL;
            double max_cost = numeric_limits<double>::max();

            // find the largest intersecting object
            for (itfobjs = fobjs.begin(); itfobjs != fobjs.end(); ++itfobjs) {
                Rect2d r2 = itfobjs->getBboxTracker();
                iou = Utils::iou(r1, r2);
                float cost1 = sqrt(pow((double) (r1.x - r2.x), 2.0) + pow((double) (r1.y - r2.y), 2.0));
                float cost2 = sqrt(pow((double) ((r1.x + r1.width) - (r2.y + r2.height)), 2.0) +
                                   pow((double) ((r1.x + r1.width) - (r2.y + r2.height)), 2.0));
                float cost = cost1 + cost2;
                if (iou > 0.2 && cost < max_cost ) {
                        best = itfobjs;
                        max_cost = cost;
                        ves_target = (*itve);
                        cout << "Found best " << best->getBboxTracker() <<  " class " << best->getClassName() << endl;
                }
            }

            // if found a match, remove any other overlapping objects. update the prediction, then erase it
            if (ves_target != NULL) {
                list<EventObject> sobjs = this->getObjects(frame_num);
                list<int> eraseobjs;
                Rect2d rbest = best->getBboxTracker();
                int idx = 0;
                iou = Utils::iou(itfobjs->getBboxTracker(), rbest);
                for (itfobjs = fobjs.begin(); itfobjs != fobjs.end(); ++itfobjs) {
                    if (best != itfobjs && iou > 0.) {
                        cout << "Object at index " << idx << " iou " << iou << " to be removed" << endl;
                        eraseobjs.push_back(idx);
                        idx += 1;
                    }
                }
                itfobjs = fobjs.begin();
                list<int>::iterator ilist;
                for (ilist = eraseobjs.begin(); ilist != eraseobjs.end(); ++ilist) {
                    std::advance(itfobjs, *ilist);
                    fobjs.erase(itfobjs);
                }
                cout << "Updating prediction" << endl;
                (*ves_target).updatePrediction(img, bin_img, *best, frame_num);

                cout << "Erasing best " << fobjs.size() << endl;
                if (fobjs.empty()) break;
                fobjs.erase(best);
            }
        }

        // add any remaining event objects that do not overlap and are not enclosing each other
        for (itsobjs = fobjs.begin(); itsobjs != fobjs.end(); ++itsobjs) {
            bool overlapping_or_enclosed = false;
            for (itve = events_.begin(); itve != events_.end(); ++itve) {
                if ((*itve)->getEndFrame() - (*itve)->getStartFrame() > 1) {
                    Rect2d r1, r2, r3;
                    r1 = itfobjs->getBboxTracker(); r2 = (*itve)->getLatestObject().getBboxTracker();
                    r3 = r1 & r2;
                    iou = Utils::iou(r1, r2);
                    if (iou > 0. || r3.area() == r2.area() || r3.area() == r1.area()) {
                        overlapping_or_enclosed = true;
                        break;
                    }
                }
            }
            Rect2d r = itsobjs->getBboxTracker();
            if (overlapping_or_enclosed == false && r.height > 10 && r.width > 10) {
                cout << "**** Adding new VisualEvent **** " << "class: " << itsobjs->getClassName() << " score: "
                     << itsobjs->getConfidence() << endl;
                addVisualEvent(img, bin_img, frame_num, *itsobjs );
            }
        }

        // update all events
        sz = events_.size();
        cout << frame_num << ":Number of VisualEvents " << sz << endl;
        for (itve = events_.begin(); itve != events_.end(); ++itve) {
            // only update if needed
            if ((*itve)->getEndFrame() != frame_num)
                (*itve)->updatePrediction(img, bin_img, frame_num);
        }

        // find largest occlusion with any given event and update the event occlusions
        for (itve = events_.begin(); itve != events_.end(); ++itve) {
            double occlusion = findLargestIntersection((*itve)->getLatestObject());
            if (occlusion > 0.)
                (*itve)->setOcclusion((float) occlusion);
        }
        cleanUp();
    }

// ######################################################################
    void VisualEventManager::addVisualEvent(const Mat &img, const Mat &bin_img, const unsigned int frame_num, const EventObject &vob) {
        uuids::uuid uuid1 = random_generator_();
        VisualEvent *evt = new VisualEvent(uuid1, img, bin_img, vob, cfg_, cfg_maps_);
        evt->updatePrediction(img, bin_img, vob, frame_num);
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
