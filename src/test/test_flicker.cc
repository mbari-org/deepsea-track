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
#include <opencv2/dnn.hpp>
#include <opencv2/saliency.hpp>
#include <opencv2/plot.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "preprocess.h"
#include "util/math_utils.h"

using namespace std;
using namespace cv;
using namespace deepsea;
using namespace saliency;

class SurpriseImage: public Mat1d {
public:
    SurpriseImage(Mat1d &img){

    };

    void init(const Mat1d& value, const Mat1d& variance)
    {

    }

    void neighborhoods(const SurpriseImage& other,
            const float neighborhood_sigma)
    {

    }
    /// get mean
    /// \return
    const Mat1d getMean() {

    }

    /// get variance
    /// \return
    const Mat1d getVariance() {

    }

};
// Model based on
// http://www.cse.psu.edu/~rtc12/CSE597E/papers/surpriseItti_Baldi05cvpr.pdf
class PoissonModel  {
public:
    PoissonModel(double sampleval, double samplevar):
            n_(1.),
            update_factor_(0.5),
            sample_value_(n_ * sampleval / (1.0 - update_factor_)),
            beta0_(1),        //n_ / (1.0 - update_factor))
        mean_(0.),
            variance_(1.){
        alpha0_ = sample_value_;
        alpha1_ = sample_value_;
        alpha2_ = sample_value_;
        beta1_  = beta0_;
        beta1_  = beta0_;
        beta_init_     = beta0_;
        expected_alpha1_=1.;expected_alpha2_=1.;
        estimated_alpha1_=0;estimated_alpha2_=1.;
     }
    // ######################################################################
    void preComputeHyperParams(const PoissonModel& sample)
    {
        alpha0_       = alpha1_;
        alpha1_       = alpha2_;
        const double update0 = alpha0_ * update_factor_;
        const double update1 = alpha1_ * update_factor_;
        const double data         = sample.sample_value_ / beta2_;
        alpha2_       = update1 + data;
        lgamma1_      = lgamma2_;
        lgamma2_      = lgamma(alpha2_);
        beta1_        = beta2_;
        expected_alpha1_ = update0 + estimated_alpha1_;
        expected_alpha2_ = update1 + estimated_alpha2_;
        estimated_alpha2_ = (sample.sample_value_ + estimated_alpha1_ * update_factor_) /
                            (1 + update_factor_);
        estimated_alpha1_ = estimated_alpha2_;
    }

    double surprise(const PoissonModel& other) {
        if (alpha1_ <= 0.0) alpha1_ = 0.0000001;
        if (alpha2_ <= 0.0) alpha2_ = 0.0000001;
        const double s = KLgamma(alpha2_, beta2_, alpha1_, beta1_, true);
        return s;
   }

    double getMean() const
    { return alpha2_ / beta2_; }

    double getVar() const
    { return alpha2_ / (beta2_ * beta2_); }

    double getUpdateFac() const
    { return update_factor_; }

    double update(std::vector<double>& vec) {
        size_t sz = vec.size();
        if (sz == 1)
            return 0.0;
        double sample_mean = std::accumulate(vec.begin(), vec.end(), 0.0) / sz;
        auto variance_func = [&sample_mean, &sz](double accumulator, const double& val) {
            return accumulator + ((val - sample_mean)*(val - sample_mean) / (sz - 1));
        };
        double sample_variance = std::accumulate(vec.begin(), vec.end(), 0.0, variance_func);

        const double mean1 = sample_mean, var1 = sample_variance / update_factor_;
        const double mean2 = sample_mean, var2 = sample_variance;

        const double ivar1 = var1 < 1.0e-10 ? 1.0e10 : 1.0 / var1;
        const double ivar2 = var2 < 1.0e-10 ? n_ * 1.0e10 : n_ / var2;

        const double update_mean = (mean1 * ivar1 + mean2 * ivar2) / (ivar1 + ivar2);
        const double update_var = 1.0 / (ivar1 + ivar2);

        const double x = update_var / var1, mm = update_mean - mean1;
        const double s = 0.5 * (x - 1.0 - log(x) + mm * mm /var1);

        mean_ = update_mean;
        variance_ = update_var;

        double ss = s * 100;// * 1e17 / M_LN2;
        if (isnan(ss))
            ss = 0.;

        return ss;
    }

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
}
};

int main( int argc, char** argv ) {
    // show help
    if(argc<2){
        cout<<
            " Usage: test_preprocess <video_name>>\n"
            " examples:\n"
            " test_preprocess $PWD/data/benthic/D0232_03HD_00-02-30.mov $PWD/data/benthic/ \n"
            << endl;
        return 0;
    }

    Mat frame;
    Mat frame_resized;
    Mat last_map;
    Mat ydata;
    Mat xdata;
    int queue_size = 90;
    std::deque<double> data_queue(queue_size);
    xdata.create(1, queue_size, CV_64F);//, CV_64F);
    ydata.create(1, queue_size, CV_64F);

    // max pooling layers
    cv::dnn::LayerParams pool;
    Size kernel(3,3); //Size(3, 3), Size(3, 2))
    Size stride(2,2);//Size(2, 2), Size(3, 2))
    Size pad(0,0);//Size(1, 1), Size(0, 1))
    pool.set("pool", "max");
    pool.set("kernel_w", kernel.width);
    pool.set("kernel_h", kernel.height);
    pool.set("stride_w", stride.width);
    pool.set("stride_h", stride.height);
    pool.set("pad_w", pad.width);
    pool.set("pad_h", pad.height);
    pool.type = "Pooling";
    pool.name = "testLayer";

    cv::dnn::Net net;
    int pool_id = net.addLayer("maxpool", "Pooling", pool);
    net.connect(0, 0, pool_id, 0);

    // set input video
    string video = argv[1];
    VideoCapture cap(video);
    double rescale_per = 0.25;
    Size scaled_size(Size(
    (int) cap.get(CAP_PROP_FRAME_WIDTH)*rescale_per,
            (int) cap.get(CAP_PROP_FRAME_HEIGHT)*rescale_per));
    Preprocess pre(scaled_size, 2, video);

    printf("Start the flicker, press ESC to quit.\n");

    bool init = false;
    Ptr<Saliency> alg = StaticSaliencyFineGrained::create();
    PoissonModel surprise(1); //>====need sample value
    int j=0;
    while(cap.read(frame)) {

        resize(frame, frame_resized, cv::Size(), rescale_per, rescale_per);

        // enhance
        Mat enhanced = pre.update(frame_resized);

        // show image with the preprocessed frame
        imshow("original", frame_resized);
        imshow("enhanced", enhanced);

        Mat hsv;
        Mat1f current_map, saliency_f;

        cvtColor(frame_resized, hsv, COLOR_BGR2HSV);  //convert to same color space as cache

        if  (!init) {
            alg->computeSaliency(hsv,  last_map);
            init = true;
        }

        alg->computeSaliency(hsv,  current_map);

        Mat1f flicker =  current_map - last_map;
        imshow("flicker", flicker);

        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setInput(cv::dnn::blobFromImage(flicker));
        Mat result = net.forward();
        result = result.reshape(1, result.total());

        // convert to vector
        std::vector<double> vec;
        double *ptr;
        for (int r = 0; r < result.rows; r++) {
            ptr = result.ptr<double>(r);
            vec.push_back((double)*ptr);
            for (int c = 0; c < result.cols; c++)  {
                ptr = result.ptr<double>(c);
                vec.push_back(*ptr);
            }
        }

        // compute surprise
        double s = surprise.update(vec);
        cout << "Surprise:" << s << endl;
        data_queue.push_back(s);

        while (data_queue.size() > queue_size)
            data_queue.pop_front();

        // plot
        for (int i = 0; i < data_queue.size(); i++) {
            double ns = data_queue[i];
            ydata.at<double>(i) = ns;
            xdata.at<double>(i) = i;
        }

        Mat plot_result;
        Ptr<plot::Plot2d> plot = plot::Plot2d::create(xdata, ydata);
        plot->setPlotSize(5, 100);
        plot->setMaxX(queue_size);
        plot->setMinX(0);
        plot->setMaxY(20);//plot->setMaxY(800);
        plot->setMinY(-1);
        plot->render(plot_result);
        imshow("surprise", plot_result);

        //quit on ESC button
        //waitKey();
        if(waitKey(1)==27)break;
        last_map = current_map;
        j += 1;

    }
    alg.release();

        return 0;
}