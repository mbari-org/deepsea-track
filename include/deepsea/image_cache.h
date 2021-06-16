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

//!\file image_cache.h  Caches images and computes running mean used in <code>preprocess.h</code>

#include <deque>
#include <opencv2/core/mat.hpp>

using namespace std;
using namespace cv;

namespace deepsea {

    template <typename T>
    class ImageCache {
    public:

        // ######################################################################
        ImageCache(unsigned int size)
                : size_(size),
                  im_size_(0, 0) {
        }

        // ######################################################################
        ~ImageCache() {
            cache_.empty();
        }

        // ######################################################################
        void push_back(const T& img)
        {
            im_size_ = Size(img.rows, img.cols);
            // quick add
            if (cache_.size() > 0) {
                sum_ += img;
                //gpu::add(sum_, im);
            } else {
                sum_ = img;
            }
            cache_.push_back(img.clone());

            while (cache_.size() > size_) {
                T f = cache_.front();
                // remove
                sum_ -= img;
                cache_.pop_front();
            }
        }

        // ######################################################################
        unsigned int size() {
            return cache_.size();
        }

        // ######################################################################
        T mean() const {

            assert(cache_.size() > 0);
            return sum_ / (float) cache_.size();
        }

    private:

        deque<T> cache_;
        T sum_;
        unsigned int size_;
        Size im_size_;
    };
}