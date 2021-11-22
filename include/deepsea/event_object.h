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

///\file event_object.h contains a representation of a visual thing.
///\see event_object contains a collection of these.

#include <opencv2/core/types.hpp>
#include <opencv2/core/mat.hpp>
#include <boost/uuid/uuid.hpp>          // uuid
#include <boost/uuid/uuid_io.hpp>       // uuids::to_string
#include <boost/lexical_cast.hpp>       // lexical_cast
#include <nlohmann/json.hpp>            // json
#include "util/voc_object.h"
#include "util/utils.h"                 // rect2json

using namespace cv;
using namespace std;
using namespace nlohmann;
using namespace boost;

namespace deepsea {

    class EventObject {

    public:

        /// \default constructor
        EventObject();

        /// \brief Construct an object from a \see voc_object
        /// \param voc
        /// \param occluded_percent
        /// \param frame
        EventObject(const VOCObject &voc, const unsigned int occluded_percent, const unsigned int frame);

        ///
        /// \return area (number of pixels) of the object
        unsigned int getArea() const;

        ///
        /// \return bounding box assigned/predicted from the tracker(s)
        inline const Rect2d getBboxTracker() const;

        ///
        /// \return name classifier assigned to this object
        inline string getClassName() const;

        ///
        /// \return index the classifier is assigned to
        inline unsigned int getClassIndex() const;

        ///
        /// \return confidence
        inline float getConfidence() const;

        ///
        /// \return surprise
        inline float getSurprise() const;

        ///
        /// \return frame number this object is in
        inline unsigned int getFrameNum() const;

        ///
        /// \return return total number of occluded pixels
        inline float getOcclusion() const;

        ///
        /// \return return the unique identifier for this object
        inline const uuids::uuid & getUuid() const;

        /// set the assigned class name to this object by the external classifier
        /// \param class_name
        void setClassName(const string class_name);

        /// set the assigned index associated with the class name to this object by the external classifier
        /// \param index
        void setClassIndex(const int index);

        /// set the class confidence betweeen 0-1, 1 being the most confident
        /// \param confidence
        void setClassConfidence(float confidence);

        /// set the percent occluded - this is really not occlusion in the pixel sense, but rather
        /// an approximation of the percent using the percent IOU from an intersecting object
        /// \param percent
        void setOcclusion(float percent);

        /// set the frame number this object belongs to
        /// \param frame_num
        void setFrameNum(const unsigned int frame_num);

        /// set the unique identifier the object belongs to
        /// \param uuid
        void setUuid(uuids::uuid uuid);

        /// set the bounding box estimated from the tracker
        /// \param bbox
        void setBboxTracker(const Rect2d bbox);

        /// set the surprise factor
        /// \param surprise
        void setSurprise(double surprise);

        /// set the total number of foreground pixels
        /// \param fgsum
        void setForegroundSum(int fgsum);

        /// \brief utility method to convert from JSON to event_object
        /// \param[in/out] j JSON object to load to
        /// \param[in] p event_object to save from
        /// \param resize_factor_width factor to rescale width
        /// \param resize_factor_height factor to rescale height
        static void to_json(json& j, const EventObject& p, float resize_factor_width, float resize_factor_height) {
            // only need to save tracker box
            Rect2d box = Utils::rescale(resize_factor_width, resize_factor_height, p.getBboxTracker());
            json jbox_t = Utils::rect2json(box);

            j = json{
                {"uuid", boost::uuids::to_string(p.uuid_)},
                {"bounding_box", jbox_t},
                {"occlusion", int(p.getOcclusion()*100.)},// scale and round to avoid bloat in JSON
                {"class_name", p.getClassName()},
                {"class_index", p.getClassIndex()},
                {"confidence", int(p.getConfidence()*100.) }, // scale and round to avoid bloat in JSON
                {"surprise",  int(p.getSurprise()/10.)}, // scale and round to avoid bloat in JSON
                {"frame_num", p.getFrameNum()}
                };
        };

        /// \brief utility method to convert from event_object to JSON
        /// \param[in] j JSON object to load from
        /// \param[in/out] p event_object to save to
        static void from_json(const json& j, EventObject& p){
            p.bbox_tracker_ = Utils::json2rect(j.at("bounding_box"));
            string uuid_str;
            int confidence, surprise, occlusion;
            j.at("uuid").get_to(uuid_str);
            p.uuid_ = boost::lexical_cast<boost::uuids::uuid>(uuid_str);
            j.at("occlusion").get_to(occlusion);
            j.at("class_name").get_to(p.class_name_);
            j.at("class_index").get_to(p.class_index_);
            j.at("confidence").get_to(confidence);
            j.at("surprise").get_to(surprise);
            j.at("frame_num").get_to(p.frame_num_);
            p.class_confidence_ = (float) confidence/100.;
            p.surprise_ = (float) surprise/10.;
            p.occlusion_ = (float) occlusion/100.;
        };

        /// \brief copy operator
        EventObject &operator=(const EventObject &object);

    private:

        float surprise_;                //! maximum surprise from surprise map; used for detecting surprising events in video for visualization and training
        Rect2d bbox_tracker_;           //! assigned/predicted bounding box in image coordinates for this object from the box tracker
        Mat mask_;                      //! binary mask with the shape of this event_object
        unsigned int frame_num_;        //! frame number at which the event_object occurred
        float occlusion_;               //! an approximation of the occlusion using the percent IOU from an intersecting object
        string class_name_;             //! class name assigned to this object by the external classifier
        int class_index_;               //! class index assigned to this object by the external classifier
        float class_confidence_;        //! confidence from the external classifier
        uuids::uuid uuid_;              ///! unique identifier for this visual event
    };

// ########### inline methods
// ######################################################################
    inline const Rect2d EventObject::getBboxTracker() const { return bbox_tracker_; }

// ######################################################################
    inline string EventObject::getClassName() const { return class_name_; }

// ######################################################################
    inline float EventObject::getConfidence() const { return class_confidence_; }

// ######################################################################
    inline float EventObject::getSurprise() const { return surprise_; }

// ######################################################################
    inline float EventObject::getOcclusion() const { return occlusion_; }

// ######################################################################
    inline unsigned int EventObject::getFrameNum() const { return frame_num_; }

// ######################################################################
    inline unsigned int EventObject::getClassIndex() const { return class_index_; }

// ######################################################################
    inline void EventObject::setClassName(const string class_name) { class_name_ = class_name;  }

// ######################################################################
    inline void EventObject::setClassIndex(const int index) { class_index_ = index; }

// ######################################################################
    inline void EventObject::setClassConfidence(float confidence) { class_confidence_ = confidence;  }

// ######################################################################
    inline void EventObject::setOcclusion(float occlusion) { occlusion_ = occlusion;  }

// ######################################################################
    inline void EventObject::setFrameNum(const unsigned int frame_num) { frame_num_ = frame_num; }

// ######################################################################
    inline void EventObject::setUuid(uuids::uuid uuid) { uuid_ = uuid; }

// ######################################################################
    inline const uuids::uuid & EventObject::getUuid() const {  return uuid_; }

// ######################################################################
    inline void EventObject::setBboxTracker(const Rect2d bbox){  bbox_tracker_ = bbox; }

// ######################################################################
    inline void EventObject::setSurprise(double surprise) { surprise_ = surprise;  }

}

