#include <algorithm>                    // std::minmax
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/saliency.hpp>         // saliency::StaticSaliencyFineGrained
#include <boost/format.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <deepsea/visual_event.h>
#include <deepsea/surprise_detector.h>

using namespace cv;
using namespace std;
using namespace saliency;

namespace deepsea {

// ######################################################################
    VisualEvent::VisualEvent(uuids::uuid uuid, const Mat &img, const Mat &bin_img,
                            const EventObject &evt_obj, const Config &cfg,
                            ConfigMaps &cfg_map)
            : uuid_(uuid),
              start_frame_(evt_obj.getFrameNum()),
              end_frame_(evt_obj.getFrameNum()),
              max_size_(evt_obj.getArea()),
              min_size_(evt_obj.getArea()),
              max_object_(evt_obj),
              state_(VisualEvent::OPEN),
              tracker_cfg_(cfg.getTracker()),
              cfg_maps_(cfg_map){
        tracker_failed_ = false;
        tracker_ = initTracker(tracker_cfg_.type);
        objects_.push_back(evt_obj);
        Mat img_clone = img.clone();
        Mat crop = img_clone(evt_obj.getBboxTracker());
        alg_ = StaticSaliencyFineGrained::create();
        computeSurprise(img.size(), crop);
        img_clone.release();
        if (tracker_cfg_.min_event_frames == 1 || tracker_cfg_.min_event_frames == 0) {
            state_ = VALID;
        }
    }

    // ######################################################################
    VisualEvent::~VisualEvent() {
        objects_.clear();
        tracker_.release();
        alg_.release();
    }

// ######################################################################
    void VisualEvent::updatePrediction(const Mat &img, const Mat &bin_img, const EventObject &evt_obj, const unsigned int frame_num) {
        tracker_failed_ = false;

        if (end_frame_ - start_frame_ > 0) {
            tracker_.release();
            tracker_ = initTracker(tracker_cfg_.type);
//            cout << "Resetting Hough Tracker " << frame_num << endl;
//            Rect box = evt_obj.getBboxTracker();
//            if (runHough(img, bin_img, box, true) == false)
//                tracker_failed_ = true;
        }

//        if (end_frame_ - start_frame_ > 1 && boundsCheck(img.size(), evt_obj.getBboxTracker(), 0.01)) {
//            cout << frame_num << ":" << boost::uuids::to_string(evt_obj.getUuid())
//                 << " Too close to edge to start tracker1" << endl;
//            tracker_failed_ = true;
//            close();
//        }
//        else {
            if (!tracker_->init(img, evt_obj.getBboxTracker())) {
                if (end_frame_ - start_frame_ > 1) {
                    ostringstream ss;
                    ss << frame_num << ":Tracking init failed for tracker" << boost::uuids::to_string(uuid_) << ","
                       << evt_obj.getClassName();
                    tracker_failed_ = true;
                }
            }

            if (tracker_failed_) {
                close();
            } else {
                VOCObject voc(evt_obj.getClassName(), evt_obj.getConfidence(), evt_obj.getBboxTracker());
                update(img, bin_img, frame_num, voc);
            }
//        }
    }


// ######################################################################
    void VisualEvent::updatePrediction(const Mat &img, const Mat &bin_img, const unsigned int frame_num) {
        Rect2d r1;
        Rect r;
        EventObject evt_obj = objects_.back();

        if (tracker_->update(img, r1)) {
//        if (runHough(img, bin_img, r, false)) {
            Size size = img.size();
//            int top = r.y;
//            int bottom = r.y + r.height;
//            int left = r.x;
//            int right = r.x + r.width;
//            r1 = Rect(left, top, (right-left), (bottom-top));

            if (boundsCheck(size, r1, .01)) {
                cout << frame_num << ":" << boost::uuids::to_string(evt_obj.getUuid())
                     << " Too close to edge - closing tracker to avoid drift" << endl;
                tracker_failed_ = true;
            }
        }
        else {
            tracker_failed_ = true;
        }
        if (tracker_failed_) {
            cout << frame_num << ":" << "Tracking failed for " << boost::uuids::to_string(evt_obj.getUuid()) << endl;
            close();
        }
        else {
//            EventObject last_evt_obj = objects_.back();
//            Rect2d r = last_evt_obj.getBboxTracker();
//            double top = r.x;
//            double top1 = r1.x;
//            double left = r.y;
//            double left1 = r1.y;
//            double bottom = r.y + r.height;
//            double bottom1 = r1.y + r1.height;
//            double right = r.x + r.width;
//            double right1 = r1.x + r1.width;
//            float max_cost = pow((double)1.3, 2.0) + 0.30*max(r.width, r.height);
//            float cost1 = (sqrt(pow((double)(top - top1),2.0) +  pow((double)(left - left1),2.0)) +
//                     sqrt(pow((double)(bottom - bottom1),2.0) + pow((double)(right - right1),2.0)));
//            if (cost1 < max_cost)
                update(img, bin_img, frame_num, VOCObject(evt_obj.getClassName(), evt_obj.getConfidence(), r1));
//            else
//                close();
        }
    }

// ######################################################################
    bool VisualEvent::boundsCheck(const Size &size, const Rect2d &bbox, const float percent) {
        int width_pad = int(percent * bbox.width);
        int height_pad = int(percent * bbox.height);
        if (bbox.x <= width_pad ||
            bbox.y <= height_pad ||
            bbox.x + bbox.width > size.width - width_pad ||
            bbox.y + bbox.height > size.height - height_pad) {
            return true;
        }
        return false;
    }

// ######################################################################
    double VisualEvent::computeSurprise(const Size &size, const Mat &crop) {

        // first check if this event is close to the edge and if so skip
        EventObject evt_obj = objects_.back();
        if (boundsCheck(size, evt_obj.getBboxTracker(), 0.01) ) {
            cout << "skipping surprise compute for " << boost::uuids::to_string(uuid_) << endl;
            prev_saliency_.release();
            return 0.;
        }

        Mat resized_image;
        float scale_w;
        float scale_h;

        // handle start-up
        Size sz = prev_saliency_.size();
        if (sz.width == 0 && sz.height == 0) {

            Size crop_size = crop.size();

            // surprise computation is demanding so rescale the dimensions to 50x50 or the dimensions of the crop if smaller
            scale_w = (float) std::min(crop_size.width, 50) / (float) crop_size.width;
            scale_h = (float) std::min(crop_size.height, 50) / (float) crop_size.height;
            cv::resize(crop, resized_image, Size(), scale_w, scale_h, INTER_AREA);

            resized_image = crop;
            alg_->computeSaliency(resized_image, prev_saliency_);

            // initialize map with queue of 30
            surprise_map_ = SurpriseMap(30, resized_image.size());
        }
        else
            resize(crop, resized_image, prev_saliency_.size(), INTER_AREA);  //resize to same as cache

        // compute saliency of image and compute flicker
        Mat1f saliency1f;
        alg_->computeSaliency(resized_image,  saliency1f);
        Mat1f flicker = saliency1f - prev_saliency_;
        prev_saliency_ = flicker;

        // convert to double as needed for surprise map
        Mat1d saliency1d;
        flicker.convertTo(saliency1d, CV_64F);
        // imshow("flicker", flicker);

        // compute surprise map and return total
        const SurpriseDetector sample(UPDFAC, saliency1d);
        Mat1d map = surprise_map_.surprise(sample);
        double surprise =  cv::sum(map)[0];
        return surprise;
    }

// ######################################################################
    void
    VisualEvent::update(const Mat &img, const Mat &bin_img, const unsigned int frame_num, const VOCObject &obj) {
        const Rect2d bbox = obj.getBox();
        const string candidate_class_name = obj.getName();
        const float candidate_class_score = obj.getScore();
        int total_frames = getEndFrame() - getStartFrame() + 1;

        string class_name = candidate_class_name;
        float class_confidence = candidate_class_score;
        float high_score = 0.f;
        float avg_score;
        int high_frames = 0;
        EventObject evt_obj;
        if (objects_.size() > 0)
            evt_obj = objects_.back();
        else
            evt_obj = EventObject();

        // add to the sum of total classes with this class name
        if (class_indexes_.count(candidate_class_name) > 0)
            class_indexes_[candidate_class_name] += 1;
        else
            class_indexes_[candidate_class_name] = 1;

        // add to the sum of the scores for that class name
        if (class_indexes_.count(candidate_class_name) > 0)
            class_confidences_[candidate_class_name] += candidate_class_score;
        else
            class_confidences_[candidate_class_name] = candidate_class_score;

        // the best class is the one with the highest average probability with at least 30% of the total
        // frames so far for this visual event. Ties are won with the class with the larger total frames.
        if (total_frames > 0) {
            int min_num = std::max(1, int(total_frames * 0.30f));
            for (map<string, int>::value_type &x : class_indexes_) {
                if (x.second >= min_num) {
                    avg_score = class_confidences_[x.first] / x.second;
                    if (avg_score >= high_score and x.second > high_frames) {
                        class_name = x.first;
                        class_confidence = avg_score;
                        high_score = avg_score;
                        high_frames = x.second;
                        cout << "Voting prediction " << class_name << boost::format(" %.2f") % class_confidence;
                        cout << " frames " << total_frames << endl;
                    }
                }
            }
        } else {
            class_confidence = candidate_class_score;
            class_name = candidate_class_name;

        }

        evt_obj.setUuid(uuid_);
        evt_obj.setFrameNum(frame_num);
        evt_obj.setClassConfidence(class_confidence);
        evt_obj.setClassName(class_name);
        evt_obj.setClassIndex(cfg_maps_.class_ids[class_name]);
        evt_obj.setOccluded(0.);
        evt_obj.setBboxTracker(bbox);

        cout << frame_num << ":Updating prediction for " <<  boost::uuids::to_string(uuid_)
            << " to " << class_name << " " << "box " << bbox << endl;

        if (evt_obj.getArea() > max_size_) {
            max_size_ = evt_obj.getArea();
            max_object_ = evt_obj;
        }
        if (evt_obj.getArea() < min_size_) {
            min_size_ = evt_obj.getArea();
        }

        end_frame_ = frame_num;

        if (bbox.x >= 0 && bbox.y >= 0 && bbox.x + bbox.width < img.cols &&  bbox.y + bbox.height < img.rows) {
            Mat crop = img(bbox);
            double surprise = computeSurprise(img.size(), crop);
            if (total_frames > 1)
                evt_obj.setSurprise(surprise);
            else
                evt_obj.setSurprise(0.);
        }

        if (total_frames >= tracker_cfg_.min_event_frames) {
            state_ = VALID;
        }
        objects_.push_back(evt_obj);
    }


// ######################################################################
    EventObject VisualEvent::getLatestObject() const {
        return objects_.back();
    }

// ######################################################################
    void VisualEvent::setOcclusion(float occlusion) {
        objects_.back().setOccluded(occlusion);
    }

// ######################################################################
    Ptr<Tracker> VisualEvent::initTracker(Config::TrackerType type) {
        if (type == Config::TT_KCF)
            return TrackerKCF::create();
        else if (type == Config::TT_TLD)
            return TrackerTLD::create();
        else if (type == Config::TT_MEDIANFLOW) {
            TrackerMedianFlow::Params params;
//            int pointsInGrid;      //!<square root of number of keypoints used; increase it to trade = 10
//            //!<accurateness for speed
//            cv::Size winSize;      //!<window size parameter for Lucas-Kanade optical flow Size(3,3)
//            int maxLevel;          //!<maximal pyramid level number for Lucas-Kanade optical flow = 5
//            TermCriteria termCriteria; //!<termination criteria for Lucas-Kanade optical flow  = type=3, maxCount=20, epsilon=0.299999
//            cv::Size winSizeNCC;   //!<window size around a point for normalized cross-correlation check (30,30)
//            double maxMedianLengthOfDisplacementDifference; //!<criterion for loosing the tracked object 10
//            params.maxMedianLengthOfDisplacementDifference
//            params.winSizeNCC = Size(5,5);
            params.maxMedianLengthOfDisplacementDifference = 8;
            params.winSize = Size(9,9);
            return TrackerMedianFlow::create(params);
        }
        else if (type == Config::TT_MOSSE)
            return TrackerMOSSE::create();
        else if (type == Config::TT_CSRT)
            return TrackerCSRT::create();
        else if (type == Config::TT_INVALID) {
            cout << "invalid tracker type" << endl;
            return NULL;
        } else {
            cout << "invalid tracker type, switching to MedianFlow tracker" << endl;
            return TrackerMedianFlow::create();
        }
    }

}