#pragma once
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

///\file  visual_event.h visual_event stores and runs tracking for a single "event" which can be anything: flora, fauna, etc.*/

#include <list>
#include <string>
#include <vector>
#include <deque>
#include <opencv2/core/mat.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/saliency.hpp>         // saliency
#include <boost/uuid/uuid.hpp>          // uuid
#include <boost/format.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "event_object.h"
#include "image_cache.h"
#include "util/config.h"
#include "util/config_class_map.h"
#include "image_cache.h"
#include "surprise_map.h"
#include "util/voc_object.h"
#include "preprocess.h"

using namespace std;
using namespace cv;
using namespace boost;
using namespace nlohmann;
using namespace saliency;

namespace deepsea {

#define BOUNDS_PERCENT 0.01f     // percent distance for tracker to be near to bounds to close out or not start

    class VisualEvent {
    public:
        /// constructor
        ///
        /// \param uuid the unique identifier string assigned to this event
        /// \param img the image the object was extracted from
        /// \param bin_img a binary image with the foreground objects labeled as 1
        /// \param evt_obj the @VisualObject to initialize this event with
        /// \param cfg system configuration
        /// \param cfg_map detection class configuration
        VisualEvent(uuids::uuid uuid, const Mat &img, const Mat &bin_img,
                    const EventObject &evt_obj, const Config &cfg,
                    ConfigMaps &cfg_map);

        /// destructor
        ~VisualEvent();

        /// updates prediction for the latest assigned object
        /// \param img
        /// \param evt_obj
        /// \param frame
        void updatePrediction(const Mat &img, const Mat &bin_img, const EventObject &evt_obj, const unsigned int frame);

        /// updates prediction for the latest assigned object
        void updatePrediction(const Mat &img, const Mat &bin_img, const unsigned int frame);

        /// \return the latest visual object added to this event
        EventObject getLatestObject() const;

        /// \brief set the occlusion for the latest EventObject
        void setOcclusion(float occlusion);

        /// \brief creates the tracker defined by the configuration file
        Ptr<Tracker> createTracker();

        /// \return total surprise image of last detected object
        inline double getSurprise();

        /// \return cropped image of last detected object
        inline Mat getCrop();

        /// \return correlation coefficient for last detected object; a proxy for pose change
        inline double getCorrelation();

        // ! visual_event states
        enum State {
            OPEN,
            VALID,
            CLOSED
        };

        /// get the state of the visual_event
        inline VisualEvent::State getState();

        /// mark this event as "closed"
        inline void close();

        /// return the unique UUID of this event
        inline uuids::uuid getUUID() const;

        /// return the frame number of the first object
        inline unsigned int getStartFrame() const;

        /// return the frame number of the last object
        inline unsigned int getEndFrame() const;

        /// return the number of frames that this event spans
        inline unsigned int size() const;

        ///return whether frame_num is between start frame and end frame
        inline bool frameInRange(const uint frame_num) const;

        ///return whether tracker has failed
        inline bool trackerFailed(const uint tracker_id) const;

    private:
        uuids::uuid uuid_;                          ///! unique identifier for this visual event
        map<string, float> class_confidences_;      ///! maps to store class confidences used in voting
        map<string, int> class_indexes_;            ///! maps to store class indexes used in voting
        deque<EventObject> objects_;
        SurpriseMap surprise_map_;                  ///! surprise map used for (post-processed) visualization and training
        Ptr<Saliency> alg_;                         ///! saliency algorithm; used to preprocess data in the surprise map
        Mat1f prev_saliency_;                       ///! output from saliency on previous image
        Mat img_crop_;
        unsigned int start_frame_;
        unsigned int end_frame_;
        int max_size_, min_size_;
        VisualEvent::State state_;
        Config::TrackerConfig tracker_cfg_;
        Ptr<Tracker> tracker_;
        bool tracker_failed_;                       ///! true if the tracker failed or is not in use
        bool tracker_init_;                         ///! true if the tracker has been initialized with a bounding box
        int pad_;                                   ///! padding in pixels - used to check if close to border and to pad objects before tracking
        ConfigMaps cfg_maps_;

        /// re/initializes trackers
        void initTracker(const Mat &img, const EventObject &vo, const unsigned int frame);

        /// updates the tracker
        void update(const Mat &img, const Mat &bin_img, const unsigned int frame_num, const VOCObject &obj);

        /// Computes surprise based on the surprise map
        /// \param[in] frame size the image map is cropped from
        /// \param[in] crop image crop to compute surprise on
        /// \return total surprise for that image crop
        double computeSurprise(const Size &size, const Mat &crop);

        /// Checks if the tracker has predicted the event is close to the frame edge
        /// \param size size of the image the tracker output should be bound within
        /// \param box tracker box
        /// \return true if close
        bool boundsCheck(const Size &size, const Rect2d &box);
    };

// ########### inline methods
// ######################################################################
    inline VisualEvent::State VisualEvent::getState() { return state_; }

// ######################################################################
    inline double VisualEvent::getSurprise() { return objects_.back().getSurprise(); }

// ######################################################################
    inline void VisualEvent::close() {
        if (tracker_ != NULL)
            tracker_.release();
        state_ = CLOSED;
        ostringstream ss;
        EventObject latest = objects_.back();
        ss << "Closing " << boost::uuids::to_string(uuid_) << "," << latest.getClassName() << ",";
        ss << boost::format("t1: %4.2f,") % latest.getBboxTracker().x <<
           boost::format("%4.2f") % latest.getBboxTracker().y  << endl;
    }
// ######################################################################
    inline uuids::uuid VisualEvent::getUUID() const { return uuid_; }

// ######################################################################
    inline unsigned int VisualEvent::getStartFrame() const { return start_frame_; }

// ######################################################################
    inline unsigned int VisualEvent::getEndFrame() const { return end_frame_; }

// ######################################################################
    inline unsigned int VisualEvent::size() const { return objects_.size(); }

// ######################################################################
    inline Mat VisualEvent::getCrop() { return img_crop_; }

// ######################################################################
    inline bool VisualEvent::frameInRange(const uint frame_num) const {
        return ((frame_num >= start_frame_) && (frame_num <= end_frame_));
    }


}