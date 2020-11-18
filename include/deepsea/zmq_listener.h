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

///\file  ZMQListener.h  Listens for events across frames*/

#include <deepsea/visual_event.h>

using namespace std;

namespace deepsea {

    class ZMQListener  {

    public:
        ZMQListener(const string address, const string topic,
                    const float resize_factor_width_,
                    const float resize_factor_height);

        ~ZMQListener();

        /// \brief listens for VOC events sent over a ZeroMQ socket
        void listen();

        //! Returns visual objects at frame_num
        list<EventObject> getObjects(unsigned int frame_num) const;

        ///! Stop listening
        void stop();

        ///! True if communication started
        bool started();

        void cleanUp(const unsigned int max_frame);

        bool initialized();

    private:

        list<EventObject> objects_;
        string address_;
        string topic_;
        bool stopped_;
        bool started_;
        bool initialized_;
        float resize_factor_height_;
        float resize_factor_width_;
    };
}