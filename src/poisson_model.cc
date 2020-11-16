#include <deepsea/util/math_utils.h>
#include <deepsea/poisson_model.h>

namespace deepsea {

        PoissonModel::PoissonModel(double value) :
                update_factor_(UPDFAC) {
            sample_value_ = value / (1.0 - update_factor_);
            beta0_ = 1.0 / (1.0 - update_factor_);
            alpha0_ = sample_value_;
            alpha1_ = sample_value_;
            alpha2_ = sample_value_;
            beta1_ = beta0_;
            beta2_ = beta0_;
        }

        void PoissonModel::update(const double sample_value) {
            alpha0_ = alpha1_;
            alpha1_ = alpha2_;
            double update1 = alpha1_ * update_factor_;
            double data = sample_value / beta2_;
            alpha2_ = update1 + data;
            this->sample_value_ = sample_value;
        }

        double PoissonModel::surprise() {
            if (alpha1_ <= 0.0) alpha1_ = 0.0000001;
            if (alpha2_ <= 0.0) alpha2_ = 0.0000001;
            const double s = KLgamma(alpha2_, beta2_, alpha1_, beta1_, true);
            return s;
        }

        double PoissonModel::getSample() const { return sample_value_; }

        double PoissonModel::getMean() const { return alpha2_ / beta2_; }

        double PoissonModel::getVar() const { return alpha2_ / (beta2_ * beta2_); }

        void PoissonModel::resetUpdateFactor(double factor) { update_factor_ = factor; }
}