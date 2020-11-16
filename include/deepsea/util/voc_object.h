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
///\file VOC.H PASCAL file parsing utility

#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

namespace deepsea {

    class VOCObject {
    public:
        VOCObject();

        VOCObject(string name, float score, Rect box);

        // Getters
        string getName() const { return name_; }

        float getScore() const { return score_; }

        Rect getBox() const { return rect_; }

        Point2d getCenter() const { return
        Point2d(rect_.x + int(rect_.width/2),
                rect_.y + int(rect_.height/2)); }

        int getIndex() const { return index_; }

        // Setters
        void setName(const std::string &name) { name_ = name; }

        void setConfidence(const float &score) { score_ = score; }

        void setBox(const Rect &rect) { rect_ = rect; }

        void setIndex(const int &index) { index_ = index; };

        //!copy operator
        VOCObject &operator=(const VOCObject &object);

    private:
        string name_;
        float score_;
        Rect rect_;
        int index_;
    };
}