#include <opencv2/opencv.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>
#include <math.h>
#include <deepsea/features.h>


using namespace std;
using namespace cv;

namespace deepsea {

    // ######################################################################
    void sobelExtractor(const Mat img, const Rect roi, Mat& feat){
        Mat sobel[2];
        Mat patch;
        Rect region = roi;

        // extract patch inside the image
        if(roi.x<0){region.x=0;region.width+=roi.x;}
        if(roi.y<0){region.y=0;region.height+=roi.y;}
        if(roi.x+roi.width>img.cols)region.width=img.cols-roi.x;
        if(roi.y+roi.height>img.rows)region.height=img.rows-roi.y;
        if(region.width>img.cols)region.width=img.cols;
        if(region.height>img.rows)region.height=img.rows;

        patch = img(region);
        cvtColor(patch,patch, COLOR_RGB2GRAY);

        // add some padding to compensate when the patch is outside image border
        int addTop,addBottom, addLeft, addRight;
        addTop=region.y-roi.y;
        addBottom=(roi.height+roi.y>img.rows?roi.height+roi.y-img.rows:0);
        addLeft=region.x-roi.x;
        addRight=(roi.width+roi.x>img.cols?roi.width+roi.x-img.cols:0);

        copyMakeBorder(patch,patch,addTop,addBottom,addLeft,addRight,BORDER_REPLICATE);

        Sobel(patch, sobel[0], CV_32F,1,0,1);
        Sobel(patch, sobel[1], CV_32F,0,1,1);

        merge(sobel,2,feat);
        feat=feat/255.0-0.5; // normalize to range -0.5 .. 0.5
    }

    // ######################################################################
    void localJetsExtractor(const Mat img, const Rect roi, const int scale, vector<double> &jet_features) {
        Mat patch;
        Rect region = roi;

        // extract patch inside the image
        if(roi.x<0){region.x=0;region.width+=roi.x;}
        if(roi.y<0){region.y=0;region.height+=roi.y;}
        if(roi.x+roi.width>img.cols)region.width=img.cols-roi.x;
        if(roi.y+roi.height>img.rows)region.height=img.rows-roi.y;
        if(region.width>img.cols)region.width=img.cols;
        if(region.height>img.rows)region.height=img.rows;

        patch = img(region);
        cvtColor(patch,patch, COLOR_RGB2GRAY);

        Mat resized_image;
        resize(patch, resized_image, Size(100, 100), INTER_AREA);

        // kernel local jet
        Mat1f gaus = Mat::zeros(1, 3, CV_32F);
        gaus.at<float>(0, 0) = .25F;
        gaus.at<float>(0, 1) = .5F;
        gaus.at<float>(0, 2) = .25F;
        Mat1f gaus_p = Mat::zeros(3, 1, CV_32F);
        gaus_p.at<float>(0, 0) = .25F;
        gaus_p.at<float>(1, 0) =.5F;
        gaus_p.at<float>(2, 0) = .25F;
        Mat1f dd = Mat::zeros(1, 3, CV_32F);
        dd.at<float>(0, 0) = -1.F;
        dd.at<float>(0, 2) = 1.F;
        Mat1f dx = dd;
        Mat1f dy = Mat::zeros(3, 1, CV_32F);
        dy.at<float>(0, 0) = -1.F;
        dy.at<float>(2, 0) = 1.F;

        Mat ima_g = resized_image;
        ima_g.convertTo(ima_g,CV_32F,1.0);
        imshow("ima_g", ima_g);

        Point2f filter_center = Point(-1, -1);
        int k = 0;
        Mat features;
        for (int s = 0; s < scale; s++) {
            vector<Mat> data;
            for (int i = 0; i < 9; i++) {
                data.push_back(Mat::zeros(100, 100, CV_32F));
            }

            Mat ima_x; filter2D(ima_g,ima_x,CV_32F,dx, filter_center, 0.0, BORDER_CONSTANT);
            imshow("ima_x", ima_x);
            Mat ima_y; filter2D(ima_g, ima_y, CV_32F, dy, filter_center, 0.0, BORDER_CONSTANT);
            imshow("ima_y", ima_y);
            Mat ima_xy; filter2D(ima_x, ima_xy, CV_32F, dy, filter_center, 0.0, BORDER_CONSTANT);
            Mat ima_xx; filter2D(ima_x, ima_xx, CV_32F, dx, filter_center, 0.0, BORDER_CONSTANT);
            imshow("ima_xx", ima_xx);
            Mat ima_yy; filter2D(ima_y, ima_yy, CV_32F, dy, filter_center, 0.0, BORDER_CONSTANT);
            imshow("ima_yy", ima_yy);
            Mat ima_xyy; filter2D(ima_yy, ima_xyy, CV_32F, dx, filter_center, 0.0, BORDER_CONSTANT);
            Mat ima_xxy; filter2D(ima_xx, ima_xxy, CV_32F, dy, filter_center, 0.0, BORDER_CONSTANT);
            Mat ima_xxx; filter2D(ima_xx, ima_xxx, CV_32F, dx, filter_center, 0.0, BORDER_CONSTANT);
            Mat ima_yyy; filter2D(ima_yy, ima_yyy, CV_32F, dy, filter_center, 0.0, BORDER_CONSTANT);

            Mat L = ima_g;
            data[0] = getInner(ima_g, 8);

            Mat LiLi = raisePower(ima_x, 2) + raisePower(ima_y, 2);
            data[1] = getInner(LiLi, 8);

            Mat a = ima_x;
            a *= ima_xx;
            a *= ima_x;
            Mat b = ima_x;
            b *= ima_xy;
            b *= ima_y;
            b *= 2;
            Mat c = ima_y;
            c *= ima_yy;
            c *= ima_y;
            Mat LiLijLj = a + b + c;
            data[2] = getInner(LiLijLj, 8);

            data[3] = getInner(ima_xx + ima_yy, 8);

            a = raisePower(ima_xx, 2);
            b = raisePower(ima_xy, 2);
            b *= 2;
            c = raisePower(ima_yy, 2);
            data[4] = getInner(a + b + c, 8);

            a *= ima_xxx;
            a *= raisePower(ima_y, 3);
            b = ima_yyy;
            b *= raisePower(ima_x, 3);
            c = ima_xyy;
            c *= raisePower(ima_x, 2);
            c *= ima_y;
            c *= 4;
            Mat d = ima_xxy;
            d *= ima_x;
            d *= raisePower(ima_y, 2);
            d *= 4;
            data[5] = getInner(a - b + c - d, 8);

            a = ima_xxy;
            a *= raisePower(ima_y, 3);
            b = ima_xxy;
            b *= ima_x;
            b *= 2;
            b *= ima_y;
            c = ima_xyy;
            c *= ima_x;
            c *= raisePower(ima_y, 2);
            d = ima_xyy;
            d *= raisePower(ima_x, 3);
            data[6] = getInner(a + b - c - d, 8);

            a = ima_xxy;
            a *= raisePower(ima_x, 3);
            a *= -1;
            b = ima_xyy;
            b *= raisePower(ima_x, 2);
            b *= ima_y;
            b *= 2;
            c = ima_yyy;
            c *= ima_x;
            c *= raisePower(ima_y, 2);
            d = ima_xxx;
            d *= ima_y;
            d *= raisePower(ima_x, 2);
            Mat e = ima_xxy;
            e *= raisePower(ima_y, 2);
            e *= ima_x;
            e *= 2;
            Mat f = ima_xyy;
            f *= raisePower(ima_y, 3);
            data[7] = getInner(a - b - c + d + e + f, 8);

            a = ima_xxx;
            a *= raisePower(ima_x, 3);
            b = ima_xxy;
            b *= raisePower(ima_x, 2);
            b *= ima_y;
            b *= 3;
            c = ima_xyy;
            c *= ima_x;
            c *= raisePower(ima_y, 2);
            c *= 3;
            d = ima_yyy;
            d *= raisePower(ima_y, 3);
            data[8] = getInner(a + b + c + d, 8);

            if (s < scale) {
                filter2D(ima_g, ima_g, CV_32F, gaus, filter_center, 0.0, BORDER_CONSTANT);
                filter2D(ima_g, ima_g, CV_32F, gaus_p, filter_center, 0.0, BORDER_CONSTANT);
            }


            // normalize
            Mat din = Mat::zeros(100, 100, CV_32F);
            for (int j = 0; j < 9; j++) {
                din = data[j];
                Scalar_<double> border_mean = computeBorderMean(din);
                cout << border_mean << endl;
                din -= border_mean;
                Scalar_<double> sumin = sum(din);
                if (sumin[0] > 0.) {
                    jet_features[k++] = negMean(din)[0];
                    cout << "negMean: " << negMean(din)[0] << endl;
                    jet_features[k++] = posMean(din)[0];
                    cout << "posMean: " << posMean(din)[0] << endl;
                    jet_features[k++] = mean(din)[0] + border_mean[0];
                    cout << "mean: " << mean(din)[0] + border_mean[0] << endl;
                } else {
                    jet_features[k++] = 0.;
                    jet_features[k++] = 0.;
                    jet_features[k++] = 0.;
                }
            }
        }
    }

    // ######################################################################
    Scalar_<double> computeBorderMean(const Mat &img) {

        int width = img.cols;
        int height = img.rows;
        float size = min(10.F, (float)round(width / 2));
        Mat mean_img = img;

        for (int i = size; i < (width - size); i++)
            for (int j = size; j < (height - size); j++)
                mean_img.at<float>(i, j, 0.0F);

        // get the mean of the remaining border
        return mean(mean_img)[0];
    }

    // ######################################################################
    Mat getInner(const Mat &img, int border) {
        int width = img.cols - border;
        int height = img.rows - border;
        Mat inner_img = img;

        for (int i = border; i < (width - border); i++)
            for (int j = border; j < (height - border); j++) {
                float val = img.at<float>(i, j);
                inner_img.at<float>(i - border, j - border, val);
            }

        return img;
    }


    // ######################################################################
    Scalar_<double> negMean(const Mat &img) {
        int width = img.cols;
        int height = img.rows;
        Mat result = Mat::zeros(img.rows, img.cols, CV_32F);

        for (int i = 0; i < width; i++)
            for (int j = 0; j < height; j++) {
                float val = img.at<float>(i, j);
                if (val < 0.f)
                    result.at<float>(i, j, val);
                else
                    result.at<float>(i, j, 0.);
            }

        return cv::mean(result);
    }

    // ######################################################################
    Scalar_<double> posMean(const Mat &img) {
        int width = img.cols;
        int height = img.rows;
        Mat result = Mat::zeros(img.rows, img.cols, CV_32F);

        for (int i = 0; i < width; i++)
            for (int j = 0; j < height; j++) {
                float val = img.at<float>(i, j);
                if (val > 0.f)
                    result.at<float>(i, j, val);
                else
                    result.at<float>(i, j, 0.);
            }

        return cv::mean(result);
    }



    // ######################################################################
    Mat raisePower(const Mat &img, int n) {

        int width = img.cols;
        int height = img.rows;
        Mat transformed_img = Mat::zeros(img.rows, img.cols, CV_32F);

        for (int i = 0; i < width; i++)
            for (int j = 0; j < height; j++) {
                float val = img.at<float>(i, j);
                transformed_img.at<float>(i, j, pow(val, n));
            }

        return transformed_img;
    }

}