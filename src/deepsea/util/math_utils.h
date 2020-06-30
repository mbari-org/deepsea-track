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
///\file math_utils.H methods for general image functions not included in opencv

//! Natural Log of 2 DEFINED in cmath as M_LN2, but is not 128 bit
#define D_LOG_2            0.6931471805599453094172321214581766
#include <math.h>
#include <cmath>                                //lgamma
#include <boost/math/special_functions.hpp>     //digamma

namespace deepsea {

    // ######################################################################
    // Slighlty modified from ilab toolkit MathFunctions.H
    // ##### Kullback-Leibler (KL) distances
    // ######################################################################
    //! Convert a number into Wows, for instance a KL distance
    /*! Wow's in this computation is the same as the conversion
      from Nats to Bits in KL
    */
    inline static double wow(const double &k) {
        // surprise should always be positive but rounding may mess that up:
        // This is OK since surprise is negative with equality if and only
        // if two distribution functions are equal. Thus, we gain no insight
        // with negative surprise.

        if (k < 0.0) return 0.0;
        // return the surprise, in units of wows (KL Bits):
        return k * (1.0F / D_LOG_2);
    }
    // ######################################################################
    // Slightly modified from version in ilab toolkit MathFunctions.H
    // ######################################################################
    //! Compute the KL distance for a single gamma PDF
    /*!
      @param a New Alpha
      @param b New Beta
      @param A Current Alpha
      @param B Current Beta
      @param doWow is true then convert output to Wows
    */
     inline static double KLgamma(const double a,const double b,
                                        const double A,const double B,
                                        const bool doWow = false)
    {
        /* We will compute the joint KL for P(x), P(X), P(y) and P(Y)
           where one is a gamma distribution and the other gaussian as:

           surprise is KL(new || old):

                                P(X)
        KL = Integrate[P(x) log[----] ]
                                P(x)

           a - New Alpha
           b - New Beta
           A - Current Alpha
           B - Current Beta

                     a B         b        Gamma[A]
        KL    = -a + --- + A Log[-] + Log[--------] + a PolyGamma[0, a] -
                      b          B        Gamma[a]

      >    A * PolyGamma[0, a]

        Note: Since the pure Gamma function can get large very quickly, we avoid
        large numbers by computing the log gamma directly as lgamma.
        */
        double d = boost::math::digamma<double>(a);
        const double k =  - a + a*B/b + A*log(b/B) + lgamma(A) - lgamma(a)
                     + (a - A)*d;

        if(doWow) return wow(k);
        else      return k;
    }
}