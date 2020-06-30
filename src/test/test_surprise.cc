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

#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/saliency.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include "preprocess.h"
#include "util/math_utils.h"
#include <math.h>

using namespace std;
using namespace cv;
using namespace deepsea;
using namespace saliency;

#define QLEN       5
#define UPDFAC     0.99
#define NEIGHSIGMA 0.5f
#define LOCSIGMA   3.0f

// Model based on
// http://www.cse.psu.edu/~rtc12/CSE597E/papers/surpriseItti_Baldi05cvpr.pdf
class PoissonModel  {

public:
    PoissonModel(double value):
            n_(1.),
            update_factor_(UPDFAC),
            mean_(0.),
            variance_(1.){
        sample_value_ = ( n_*value )/ (1.0 - update_factor_);
        beta0_ = n_ / (1.0 - update_factor_);
        alpha0_ = sample_value_;
        alpha1_ = sample_value_;
        alpha2_ = sample_value_;
        beta1_  = beta0_;
        beta2_  = beta0_;
        beta_init_     = beta0_;
        expected_alpha1_=1.;expected_alpha2_=1.;
        estimated_alpha1_=0;estimated_alpha2_=0.;
    }
    void update(const double sample_value)
    {
        alpha0_       = alpha1_;
        alpha1_       = alpha2_;
        double update0 = alpha0_ * update_factor_;
        double update1 = alpha1_ * update_factor_;
        double data         = sample_value / beta2_;
        /*//itsAlpha2       = alphaUpdate1 + data;
        //if(alphaUpdate1 > 0)
        //  {
        //    itsAlpha2       = alphaUpdate1 +
        //      (log(itsBeta2) - psi(alphaUpdate1) + log(sample.itsSample)) /
        //     itsBeta1;
        //  }
        //else
        //  {
        itsAlpha2       = alphaUpdate1 + data;
        //  }*/
        alpha2_       = update1 + data;
        lgamma1_      = lgamma2_;
        lgamma2_      = lgamma(alpha2_);
        beta1_        = beta2_;
        expected_alpha1_ = update0 + estimated_alpha1_;
        expected_alpha2_ = update1 + estimated_alpha2_;
        estimated_alpha2_ = (sample_value + estimated_alpha1_*update_factor_) / (1 + update_factor_);
        estimated_alpha1_ = estimated_alpha2_;
        this->sample_value_ = sample_value;
    }
    void combineFrom(const vector<PoissonModel>& models,
                                             const Mat1f& weights,
                                             const Point2i& pos,
                                             const int width,
                                             const int height,
                                             const int offset)
    { 

        double wsum = 0.0F; // find out how much total weight we use
        double cov  = 0.0F; // the covariance term
        unsigned int w=0;

        // define bounds of our scan in the weight mask and go for it:
        w = offset - pos.x;
        if((expected_alpha2_ != 0) && (expected_alpha1_ != 0))        {
            for (ushort j = 0; j < height; j++)            {
                for (ushort i = 0; i < width; i++)                {
                    const double weight = (double) weights.at<float>(j, w);
                    if (weight)                    {
                        // We use floats since precision in this part is less important
                        // and we may be able to fit all the terms into registers at
                        // the same time which will speed this up a whole lot.
                        const double aRatio2 = models[i+j*width].expected_alpha2_ / expected_alpha2_;
                        const double aRatio1 = models[i+j*width].expected_alpha1_ / expected_alpha1_;
                        const double ratDiff = aRatio2 - aRatio1;
                        cov  += (weight * ratDiff) * alpha1_;
                        wsum += weight;
                    }
                    w += 1;
                }
                w = offset - pos.x;
            }
        }
        else {
            cov = alpha2_;
            wsum = 1;
        }
        const double combinedAlpha2 = cov            / wsum;
        const double ratio          = combinedAlpha2 / alpha2_;
        if((alpha2_ != 0) && (ratio != 0))
            beta2_ = beta_init_ * ((ratio * beta1_ * update_factor_) + 1);
        else
            beta2_ = beta1_ * update_factor_ + 1;
    }
    double surprise() {
        if (alpha1_ <= 0.0) alpha1_ = 0.0000001;
        if (alpha2_ <= 0.0) alpha2_ = 0.0000001;
        const double s = KLgamma(alpha2_, beta2_, alpha1_, beta1_, true);
        return s;
    }
    double getSample() const
    { return sample_value_; }
    double getMean() const
    { return alpha2_ / beta2_; }
    double getVar() const
    { return alpha2_ / (beta2_ * beta2_); }
    double getUpdateFac() const
    { return update_factor_; }
    void resetUpdateFactor(double factor)
    { update_factor_ = factor; }
private:

    double sample_value_;
    double n_;
    double mean_;
    double variance_;
    double update_factor_;
    double alpha0_;      //!< iteration 0 Gamma alpha
    double alpha1_;      //!< iteration 1 Gamma alpha
    double alpha2_;      //!< iteration 2 Gamma alpha
    double beta0_;       //!< iteration 0 Gamma beta
    double beta1_;       //!< iteration 1 Gamma beta
    double beta2_;       //!< iteration 2 Gamma beta
    double beta_init_;    //!< Initial Value of beta
    double expected_alpha1_;//!< The expected value of alpha1
    double expected_alpha2_;//!< The expected value of alpha2
    double estimated_alpha1_;       //!< The estimated value of alpha1
    double estimated_alpha2_;       //!< The estimated value of alpha2
    double lgamma1_;     //!< Computing lgamma is expensive, so we keep it
    double lgamma2_;     //!< Computing lgamma is expensive, so we keep it
};

class SurpriseDetector {
public:

    SurpriseDetector(const Size &sz):
            img_(sz, CV_64F) {
        for (int r = 0; r < img_.rows; r++) {
            for (int c = 0; c < img_.cols; c++) {
                models_.push_back(PoissonModel(0.f));
            }
        }
    }
    SurpriseDetector(double update_factor, const Mat1d& sample):
            update_factor_(update_factor),
            img_(sample.size(), CV_64F) {
        for(int i = 0; i < sample.rows; i++)
        {
            const double* Mi = sample.ptr<double>(i);
            for(int j = 0; j < sample.cols; j++)
                models_.push_back(PoissonModel(Mi[j]));
        }
    }
    void resetUpdateFactor(double factor) {
        for (std::vector<PoissonModel>::iterator it = models_.begin() ; it != models_.end(); ++it) {
            (*it).resetUpdateFactor(factor);
        }
    }
    void update(const SurpriseDetector& other) {
        std::vector<PoissonModel> models = other.getModels();
        int i = 0;
        assert(other.getSize() == this->img_.size());
        for (std::vector<PoissonModel>::iterator it = models_.begin() ; it != models_.end(); ++it) {
            (*it).update(models.at(i).getSample());
            i += 1;
        }
    }
    Mat1d surprise(const SurpriseDetector& other) {
        int z = 0;
        Mat1d map(other.getSize(), CV_64F);
        for(int i = 0; i < map.rows; i++)
        {
            double* Mi = map.ptr<double>(i);
            for(int j = 0; j < map.cols; j++) {
                Mi[j] = models_.at(z).surprise();
                z += 1;
            }
        }
        return map;
    }
    void neighborhoods(const SurpriseDetector& image, const Mat1f& weights)
    {
        Size sz = img_.size();
        assert(weights.size().width  == 2 * sz.width  + 1);
        assert(weights.size().height == 2 * sz.height + 1);
        const int w = sz.width, h = sz.height;
        Point2i pos;
        for (int j = 0; j < h; j ++) {
            const int o = w + (2 * w + 1) * (h - j);
            for (int i = 0; i < w; i++) {
                models_.at(i+j*w).combineFrom(image.getModels(), weights, Point2i(i,j), w, h, o);
            }
        }
    }
    Size getSize() const {
        return img_.size();
    }
    Mat1d getImage() const {
        return img_;
    }
    std::vector<PoissonModel> getModels() const {
        return models_;
    }
private:
    std::vector<PoissonModel> models_;
    Mat1d img_;
    double update_factor_;
};

class SurpriseMap
{
public:
    SurpriseMap() {

    }
    SurpriseMap(const SurpriseDetector& sample)
    {
        SurpriseDetector  image(sample.getSize());
        Size dims = sample.getSize();
        const int w = dims.width, h = dims.height;
        for (uint i = 0; i < QLEN; i ++)
            surprise_images_.push_back(image);

        const Size          d(w * 2 + 1, h * 2 + 1);
        const Point2i       p(w, h);
        weights_ = gaussianBlob(d, p, NEIGHSIGMA, NEIGHSIGMA);
        weights_ -= gaussianBlob(d, p, LOCSIGMA, LOCSIGMA) * (LOCSIGMA * LOCSIGMA / (NEIGHSIGMA * NEIGHSIGMA) * 1.5f);

        // remove negative values and remove low weights
        double min, max, low_val;
        minMaxLoc(weights_, &min, &max);
        low_val = 0.01*max;
        for(int i = 0; i < weights_.rows; i++)
        {
            double* Mi = weights_.ptr<double>(i);
            for(int j = 0; j < weights_.cols; j++) {
                if (Mi[j] < 0 || Mi[j] <= low_val)
                    Mi[j] = 0;
            }
        }
    }
    Mat1f gaussianBlob(const Size& dims, const Point2i& center, const float sigmaX, const float sigmaY)
    {
        Mat1f ret(dims, CV_32F);
        const int w = dims.width, h = dims.height;

        const float fac = 0.5f / (M_PI * sigmaX * sigmaY);
        const float vx = -0.5f / (sigmaX * sigmaX);
        const float vy = -0.5f / (sigmaY * sigmaY);
        for (int jj = 0; jj < h; jj ++)
        {
            float vydy2 = float(jj - center.y); vydy2 *= vydy2 * vy;
            for (int ii = 0; ii < w; ii ++)
            {
                float dx2 = float(ii - center.x); dx2 *= dx2;

                float v =  (fac * expf(vx * dx2 + vydy2));
                ret.at<float>(jj,ii) = v > 0 ? v: 0;
            }
        }

        return ret;
    }
    Mat1d surprise(const SurpriseDetector& image) {

        Mat1d surprise_map;
        SurpriseDetector input = image;
        Size sz = image.getSize();

        int i = 0;
        double min, max;
        for (std::vector<SurpriseDetector>::iterator it = surprise_images_.begin() ; it != surprise_images_.end(); ++it) {

            (*it).resetUpdateFactor(UPDFAC);

            (*it).update(input);

            //(*it).neighborhoods(input, weights_);

            Mat1d total_surprise(sz, CV_64F);

            // update local models and compute the local temporal surprise:
            const Mat1d local_surprise = (*it).surprise(input);

            total_surprise = local_surprise;

            // total surprise is multiplicative across models with different time scales
            if (i == 0)
                surprise_map = total_surprise;
            else
                surprise_map = surprise_map.mul(total_surprise);

            Point2i pt(sz.width/2, sz.height/2);
            double mean = input.getModels().at(pt.x+pt.y*sz.width).getMean();
            double var = input.getModels().at(pt.x+pt.y*sz.width).getVar();
            double val = surprise_map.at<double>(pt);
            minMaxLoc(surprise_map, &min, &max);
            printf("==========>MODELS: %d   %g %g  SURPRISE: %g min:%g max: %g\n", i, mean, var, val, min, max);

            // the updated models are the input to the next iteration:
            input = (*it);
            i += 1;
        }

        // calm down total surprise and preserve units of wows
        Mat final_map;
        cv::pow(surprise_map, 1.0 / (3.0 * double(surprise_images_.size())), final_map);

        final_map *= 5;
        return final_map;
    }

    const SurpriseDetector& getSurpriseImage(const uint index) const;

private:
    std::vector< SurpriseDetector > surprise_images_;
    Mat1f weights_;
};
int main( int argc, char** argv ) {
    // show help
    if(argc<2){
        cout<<
            " Usage: test_surprise <video_name>>\n"
            " examples:\n"
            " test_surprise $PWD/data/benthic/D0232_03HD_00-02-30.mov \n"
            << endl;
        return 0;
    }


    // set input video
    string video = argv[1];
    VideoCapture cap(video);
    const int video_width = cap.get(CAP_PROP_FRAME_WIDTH);
    const int video_height = cap.get(CAP_PROP_FRAME_HEIGHT);
    double rescale_per_w = 320./video_width;
    double rescale_per_h = 240./video_height;
    Size scaled_size(Size((int) video_width*rescale_per_w, (int)video_height*rescale_per_h));
    Preprocess pre(scaled_size, 2, video);
    Ptr<Saliency> alg = StaticSaliencyFineGrained::create();
    Mat frame;
    Mat frame_resized;
    Mat1f saliency_32f;
    Mat1d saliency_64f;
    Mat enhanced;
    Mat lab;
    Mat1d resized_surprise_map;
    Mat1f resized_saliency_map;
    SurpriseMap surprise_map(scaled_size);
    Mat1f last_image;
    Mat flicker;
    bool init = false;

    printf("Start the surprise computation, press ESC to quit.\n");
    while(cap.read(frame)) {

        resize(frame, frame_resized, cv::Size(), rescale_per_w, rescale_per_h);

        // enhance
        Mat enhanced = pre.update(frame_resized);
        
        if (!init) {
            alg->computeSaliency(enhanced,  last_image);
            init = false;
        }

        // compute saliency of image and compute flicker
        alg->computeSaliency(enhanced,  saliency_32f);
        flicker = saliency_32f - last_image;

        // convert to double as needed for surprise map
        saliency_32f.convertTo(saliency_64f, CV_64F);
        resize(saliency_32f, resized_saliency_map, cv::Size(), 1./rescale_per_w, 1./rescale_per_h);

        // run surprise
        SurpriseDetector sample(UPDFAC, saliency_64f);
        Mat1d map = surprise_map.surprise(sample);
        resize(map, resized_surprise_map, cv::Size(), 1./rescale_per_w, 1./rescale_per_h);

        // display
        imshow("saliency_map", resized_saliency_map);
        imshow("flicker", flicker);
        imshow("frame", frame);
        imshow("saliency_32f", saliency_32f);
        imshow("surprise_map", resized_surprise_map);

        //quit on ESC button
        //waitKey();
        if(waitKey(1)==27)break;
        last_image = saliency_32f;
    }
    alg.release();

    return 0;
}