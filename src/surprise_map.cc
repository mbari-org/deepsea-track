#include <algorithm>
#include <deepsea/surprise_map.h>

namespace deepsea {
// ######################################################################
    SurpriseMap::SurpriseMap()
    {}

// ######################################################################
    SurpriseMap::SurpriseMap(const unsigned int queue_length, const Size& sz)
    {
        assert(queue_length > 0);
        SurpriseDetector  detector(sz);
        for (uint i = 0; i < queue_length; i ++)
            detectors_.push_back(detector);
    }

// ######################################################################
    Mat1d SurpriseMap::surprise(const SurpriseDetector& image) {

        Mat1d surprise_map;
        SurpriseDetector input = image;
        Size sz = image.getSize();

        int i = 0;
        double min, max;
        for (vector<SurpriseDetector>::iterator it = detectors_.begin() ; it != detectors_.end(); ++it) {

            (*it).resetUpdateFactor(UPDFAC);

            (*it).update(input);

            Mat1d total_surprise(sz, CV_64F);

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
            //printf("==========>MODELS: %d   %g %g  SURPRISE: %g min:%g max: %g\n", i, mean, var, val, min, max);

            // the updated models are the input to the next iteration:
            input = (*it);
            i += 1;
        }

        // calm down total surprise and preserve units of wows
        Mat final_map;
        pow(surprise_map, 1.0 / (3.0 * double(detectors_.size())), final_map);

        final_map *= 5;
        return final_map;
    }

}