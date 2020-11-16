#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/videoio.hpp>
#include <math.h>                   //pow
#include <stdio.h>
#include <deepsea/preprocess.h>

using namespace std;
using namespace cv;

namespace deepsea {

    Preprocess::Preprocess(const Size dims, unsigned int cache_size, string video)
            : num_processed_(0),
              entropy_(0.),
              cache_(cache_size) {
        for (int i = 0; i < 256; i++) pdf_[i] = 0.F;

        Mat3f f;
        VideoCapture cap(video);
        unsigned int num_frames = 0;
        Mat frame;
        Mat frame_resized;
        float scale_width = (float) dims.width / (float) cap.get(CAP_PROP_FRAME_WIDTH);
        float scale_height = (float) dims.height / (float) cap.get(CAP_PROP_FRAME_HEIGHT);
        while (cap.read(frame)) {
            resize(frame, frame_resized, Size(), scale_width, scale_height);
            update(frame_resized);
            num_frames += 1;
            if (num_frames > cache_size)
                break;
        }
        if (num_frames < cache_size)
            printf("Less input frames than necessary for sliding average - "
                   "using all the frames for caching.");
        frame.release();
        frame_resized.release();
    }

    Preprocess::~Preprocess() {}

    Mat Preprocess::update(const Mat &img) {
        // if first frame update gamma correction curve
        if (cache_.size() == 0) {
            updateGammaCurve(img, true);
        } else {
            // update model on entropy shifts
            if (num_processed_ == 0) {
                float entropy = computeEntropy(img);

                if (!isinf(entropy) && !isnan(entropy)) {
                    float diff = abs(entropy - entropy_);
                    if (diff > 3.0) {
                        printf("Frame: %d Updating gamma curve. Entropy diff %f \n", num_processed_, diff);
                        updateGammaCurve(img, true);
                    }
                    entropy_ = entropy;
                }
            }
            updateGammaCurve(img, false);
        }
        Mat gray, hsv, lab, color, laser_mask;
        cvtColor(img, hsv, COLOR_BGR2HSV);
        cvtColor(img, gray, COLOR_BGR2GRAY);

        Vec3b *ptr_hsv;
        for (int r = 0; r < hsv.rows; r++) {
            ptr_hsv = hsv.ptr<Vec3b>(r);
            for (int c = 0; c < hsv.cols; c++) {
                float gamma = 1 - cdfw_[int(gray.at<uchar>(r, c))];
                // apply gamma in the value only, preserving hue and saturation
                ptr_hsv[c] = Vec3b(ptr_hsv[c][0], ptr_hsv[c][1], int(255 * pow(ptr_hsv[c][2] / 255.0F, gamma)));
            }
        }

        cvtColor(hsv, color, COLOR_HSV2BGR);
        cvtColor(color, lab, COLOR_BGR2Lab);
        Mat1b bin_mask = Mat::zeros(color.size(), CV_8U);

        // mean from the cache
        Mat3b mean_img;
        if (cache_.size() > 0) {
            const Mat3f mean_imgf = cache_.mean();
            mean_imgf.assignTo(mean_img, CV_8UC3);
        } else
            mean_img = color;

        Vec<double, 4> mean_pix = mean(mean_img);

        // create a mask based where the L*a*b color space where red has strong positive alpha
        float threshhold_a = 50.F;
        Vec3b *ptr;
        for (int r = 0; r < lab.rows; r++) {
            ptr = lab.ptr<Vec3b>(r);
            for (int c = 0; c < lab.cols; c++) {
                float a = ptr[c][1] / 3.0f; // 1/3 weight
                if (a > threshhold_a)
                    bin_mask.at<uchar>(r, c) = 255;
            }
        }
        // dilate it and apply mean to all values
        int erosion_size = 2;
        Mat1b element = getStructuringElement(MORPH_ELLIPSE,
                                            Size(2 * erosion_size + 1, 2 * erosion_size + 1),
                                            Point(erosion_size, erosion_size));
        Mat1b bin_mask_dilated;
        dilate(bin_mask, bin_mask_dilated, element);
        for (int r = 0; r < color.rows; r++) {
            for (int c = 0; c < color.cols; c++) {
                if (bin_mask_dilated.at<uchar>(r, c) > 0)
                    color.at<Vec3b>(r, c) = Vec3b((int) mean_pix[0], (int) mean_pix[1], (int) mean_pix[2]);
            }
        }

        bin_mask_dilated.release();
        bin_mask.release();
        gray.release();
        hsv.release();
        lab.release();
        num_processed_ += 1;
        Mat3f color_f;
        color.assignTo(color_f, CV_32F);
        cache_.push_back(color_f);

        return color;
    }

    Mat Preprocess::computeHist(const Mat &img) {
        Mat gray;
        Mat hist;
        float range[] = {0, 256};
        const float *h_range[] = {range};
        //size of the histogram -1D histogram
        int h_size = 256;
        int channels = 0;
        _InputArray mask;
        cvtColor(img, gray, COLOR_BGR2GRAY);
        calcHist(&img, 1, &channels, mask, hist, 1, &h_size, h_range);
        gray.release();
        return hist;
    }

    void Preprocess::updateGammaCurve(const Mat &img, bool init) {
        map<int, double> pdfw, cdfw;
        float pdf_min = 1.f;
        float pdf_max = 0.f;

        if (init) {
            Size s = img.size();
            float ttl = s.height * s.width;
            Mat hist = computeHist(img);

            for (int i = 0; i < 256; i++) {
                // fast pdf approximation
                pdf_[i] = hist.at<float>(i) / ttl;
                if (pdf_[i] < pdf_min)
                    pdf_min = pdf_[i];
                if (pdf_[i] > pdf_max)
                    pdf_max = pdf_[i];
            }
            // release memory
            hist.release();
        } else {
            for (int i = 0; i < 256; i++) {
                if (pdf_[i] < pdf_min)
                    pdf_min = pdf_[i];
                if (pdf_[i] > pdf_max)
                    pdf_max = pdf_[i];
            }
        }

        // fast weighting distribution
        float alpha = 0.25f;
        float sumpdfw = 0.f;
        for (int i = 0; i < 256; i++) {
            pdfw[i] = pdf_max * pow((pdf_[i] - pdf_min) / (pdf_max - pdf_min), alpha);
            sumpdfw += pdfw[i];
        }

        // modified cumulative distribution function
        for (int i = 0; i < 256; i++)
            for (int k = 0; k < i; k++)
                cdfw[i] += pdfw[k] / sumpdfw;

        cdfw_ = cdfw;
    }

    float Preprocess::computeEntropy(const Mat &img) {
        Size s = img.size();
        float ttl = s.height * s.width;
        std::map<int, double> pdf;
        float entropy = 0.f;

        Mat hist = computeHist(img);

        // fast pdf approximation
        for (int i = 0; i < 256; i++)
            pdf[i] = hist.at<float>(i) / ttl;

        // calculate entropy
        float H = 0.f;
        for (int i = 0; i < 256; i++) {
            if (pdf[i] > 0.F)
                H += pdf[i] * log(pdf[i]);
        }

        hist.release();
        entropy = -1 * H;
        return entropy;
    }
}