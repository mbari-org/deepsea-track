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
#include <zmq.hpp>

using namespace std;
using namespace zmq;

namespace deepsea {

    class ZMQListener  {

    public:
        ZMQListener(const string address, const float tracker_width, const float tracker_height);

        ~ZMQListener();

        /// \brief listens for VOC events sent over a ZeroMQ socket
        void listen(list<EventObject> &objects, unsigned int frame_num);

        /// \brief initializes ZeroMQ socket
        void init();

         //! True if this is a valid zmq object that should be initialized
        bool valid();

    private:

        context_t context_;
        socket_t server_;
        string address_;
        bool started_;
        bool valid_;
        float tracker_height_;
        float tracker_width_;
    };
}