#include "surprise_detector.h"
#include "util/math_utils.h"

namespace deepsea {

// ######################################################################
    SurpriseDetector::SurpriseDetector(const Size &sz) :
            img_(sz, CV_64F) {
        for (int r = 0; r < img_.rows; r++) {
            for (int c = 0; c < img_.cols; c++) {
                models_.push_back(PoissonModel(0.f));
            }
        }
    }

// ######################################################################
    SurpriseDetector::SurpriseDetector(double update_factor, const Mat1d &sample) :
            update_factor_(update_factor),
            img_(sample.size(), CV_64F) {
        for (int i = 0; i < sample.rows; i++) {
            const double *Mi = sample.ptr<double>(i);
            for (int j = 0; j < sample.cols; j++)
                models_.push_back(PoissonModel(Mi[j]));
        }
    }

// ######################################################################
    void SurpriseDetector::resetUpdateFactor(double factor) {
        for (vector<PoissonModel>::iterator it = models_.begin(); it != models_.end(); ++it) {
            (*it).resetUpdateFactor(factor);
        }
    }

// ######################################################################
    void SurpriseDetector::update(const SurpriseDetector &other) {
        vector <PoissonModel> models = other.getModels();
        int i = 0;
        assert(other.getSize() == this->img_.size());
        for (vector<PoissonModel>::iterator it = models_.begin(); it != models_.end(); ++it) {
            (*it).update(models.at(i).getSample());
            i += 1;
        }
    }

// ######################################################################
    Mat1d SurpriseDetector::surprise(const SurpriseDetector &other) {
        int z = 0;
        Mat1d map(other.getSize(), CV_64F);
        for (int i = 0; i < map.rows; i++) {
            double *Mi = map.ptr<double>(i);
            for (int j = 0; j < map.cols; j++) {
                Mi[j] = models_.at(z).surprise();
                z += 1;
            }
        }
        return map;
    }

// ######################################################################
    Size SurpriseDetector::getSize() const {
        return img_.size();
    }

// ######################################################################
    Mat1d SurpriseDetector::getImage() const {
        return img_;
    }

// ######################################################################
    vector <PoissonModel> SurpriseDetector::getModels() const {
        return models_;
    }
}
