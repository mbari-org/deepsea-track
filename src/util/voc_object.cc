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
 * This is a program to automate detection and tracking of events in underwater
 * video.
 *
 * This work would not be possible without the generous support of the
 * David and Lucile Packard Foundation
 */

#include <deepsea/util/voc_object.h>

namespace deepsea {
// ######################################################################
    VOCObject::VOCObject()
            : name_(""),
              score_(0.f),
              index_(-1),
              rect_(Rect(0, 0, 0, 0)) {
    }

// ######################################################################
    VOCObject::VOCObject(string name, float score, Rect box)
        : name_(name),
                score_(score),
                index_(-1),
                rect_(box) {
    }

// ######################################################################
    VOCObject &VOCObject::operator=(const VOCObject &object) {
        this->rect_ = object.rect_;
        this->name_ = object.name_;
        this->score_ = object.score_;
        this->index_ = object.index_;
        return *this;
    }

}