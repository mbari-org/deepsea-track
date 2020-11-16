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

///\file  VisualEventManager.h  Manages visual events across frames*/

#include <list>
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include "util/voc_object.h"
#include "visual_event.h"
#include "event_object.h"
#include "util/config_class_map.h"

using namespace std;
using namespace cv;
using namespace boost;

namespace deepsea {

    class VisualEventManager {

    public:
        VisualEventManager(const Config &cfg, ConfigMaps &cfg_map);

        ~VisualEventManager();

        /// \brief creates and destroys VisualEvents
        /// \param evt_objs
        /// \param img
        /// \param frame_num
        void run(list<EventObject> &evt_objs, const Mat &img, const unsigned int frame_num);

        list<EventObject> runNMS(const list<EventObject> &vobs);

        bool getPredictedBox(VisualEvent *ve, const Size size, Rect2d &pred);

        //! Returns pointer to all events at frame_num
        list<VisualEvent *> getEvents(unsigned int frame_num) const;

        //! Returns visual objects at frame_num
        list<EventObject> getObjects(unsigned int frame_num) const;

    private:
        /// \brief find the event that best matches the given EventObject
        /// \param img
        /// \param evt_obj
        /// \param cc
        /// \return
        VisualEvent *findEvent(const Mat &img, const EventObject &evt_obj, double &cc);

        /// \brief find the largest intersection of any event with the event object
        /// \param evt_obj event object to search for intersections
        /// \return
        double findLargestIntersection(const EventObject &evt_obj);

        void cleanUp();

        list<VisualEvent *> events_;
        uuids::random_generator random_generator_;
        ConfigMaps cfg_maps_;
        Config cfg_;

        void addVisualEvent(const Mat &img, const unsigned int frame_num, const EventObject &vob);
    };
}