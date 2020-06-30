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

#include <vector>
#include <math.h>

using namespace std;

#define UPDFAC     0.99

// Model based on
// http://www.cse.psu.edu/~rtc12/CSE597E/papers/surpriseItti_Baldi05cvpr.pdf
// uses time surprise only
namespace deepsea {

    class PoissonModel {

    public:

        PoissonModel(double value);

        void update(const double sample_value);

        double surprise();

        double getSample() const;

        double getMean() const;

        double getVar() const;

        void resetUpdateFactor(double factor);

    private:
        double sample_value_;
        double update_factor_;
        double alpha0_;      //!< iteration 0 Gamma alpha
        double alpha1_;      //!< iteration 1 Gamma alpha
        double alpha2_;      //!< iteration 2 Gamma alpha
        double beta0_;       //!< iteration 0 Gamma beta
        double beta1_;       //!< iteration 1 Gamma beta
        double beta2_;       //!< iteration 2 Gamma beta
    };
}