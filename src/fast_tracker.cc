#include <deepsea/fast_tracker.h>
#include <opencv2/tracking.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/cvstd.hpp>

using namespace cv;
using namespace std;

namespace deepsea {

// ######################################################################
    FastTracker::FastTracker(const Ptr<Tracker>& tracker, const int stride)
            : tracker_(tracker), frame_cnt_(0), stride_(std::min(stride, 4)), res_(true) {
    }

// ######################################################################
    FastTracker::~FastTracker()  {}

// ######################################################################
    void FastTracker::init(InputArray image, const Rect& boundingBox)
    {
        tracker_->init(image, static_cast<Rect2d>(boundingBox));
    }

// ######################################################################
    bool FastTracker::update(InputArray image, Rect& boundingBox)
    {
        if (frame_cnt_ == 0 || (frame_cnt_ % stride_) == 0) {
            res_ = tracker_->update(image, box_);
        }
        frame_cnt_ += 1;
        boundingBox = box_;
        return res_;
    }

// ######################################################################
    Ptr<Tracker> FastTracker::create(const Config::TrackerConfig config, const float occlusion)
    {
        if (config.type == Config::TT_KCF) {
            cout << "Initializing KCF tracker" << endl;
            TrackerKCF::Params param;
            //example of how to insert custom feature extractor - must be linear to compute euclidean distance
            //            param.desc_pca = TrackerKCF::GRAY | TrackerKCF::CN;
            //            param.desc_npca = 0;
            //            param.compress_feature = true;
            //            param.compressed_size = 2; //becomes 1
            //            tracker->setFeatureExtractor(sobelExtractor);
            param.desc_pca = TrackerKCF::CN;
            param.desc_npca = TrackerKCF::CN;
            param.resize = true;  // activate the resize feature to improve the processing speed
            param.detect_thresh = .125;  // detection confidence threshold; default 0.5
            param.max_patch_size = 40*40;  // threshold for the ROI size; default is 80x80
            param.pca_learning_rate = (occlusion > 0.25? 0: 0.05f); //default is 0.2, otherwise don't learn when occluded by .25
            return makePtr<FastTracker>(TrackerKCF::create(param), config.stride);
        }
        else if (config.type == Config::TT_CSRT) {
            TrackerCSRT::Params param;
            param.use_color_names = true;
            param.use_hog = true;
            param.use_rgb = true;
            param.use_gray = false;
            param.template_size = 70;  // for speed-up; default is 200
            param.admm_iterations = 2;  // for speed-up; default is 4
            cout << "Initializing CSRT tracker" << endl;
            return makePtr<FastTracker>(TrackerCSRT::create(param), config.stride);
        }
        else {
            assert("invalid tracker type");
        }
        return NULL;
    }

}