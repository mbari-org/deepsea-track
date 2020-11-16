#include <deepsea/util/utils.h>

namespace deepsea {

    double Utils::iou(const Rect_<float> bb1, const Rect_<float> bb2) {
        float in = (bb1 & bb2).area();
        float un = bb1.area() + bb2.area() - in;

        if (un < DBL_EPSILON)
            return 0.;

        double iou = (double) (in / un);
        return iou;
    }

    Rect2d Utils::rescale(float resize_factor_width, float resize_factor_height, const Rect2d &bbox) {
        Rect2d b;
        b.x = int(bbox.x * 1. / resize_factor_width);
        b.y = int(bbox.y * 1. / resize_factor_height);
        b.width = int(bbox.width * 1. / resize_factor_width);
        b.height = int(bbox.height * 1. / resize_factor_height);
        return b;
    }


    json Utils::rect2json(const Rect2d &box){
        json j =  {
                    {"x", int(box.x)},
                    {"y", int(box.y)},
                    {"width", int(box.width)},
                    {"height", int(box.height)}
                    };
        return j;
    }

    Rect2d Utils::json2rect(const json &j){
        Rect2d r;
        j.at("x").get_to(r.x);
        j.at("y").get_to(r.y);
        j.at("width").get_to(r.width);
        j.at("height").get_to(r.height);
        return r;
    }

    void Utils::decorate(Mat frame, const Rect2d bbox, const Scalar color,
                         const string uuid, const string description, int thickness_box, float font_scale) {
        // overlay location and adjustments to keep the string in view
        Point2d pt1(bbox.x - 25, bbox.y - 25);
        if (pt1.y < int(0.1 * bbox.height)) pt1.y = pt1.y + bbox.height + 25;  // adjust if too close to the top
        if (pt1.x + bbox.width > int(0.9 * bbox.width)) pt1.x -= 50; // adjust if too close to the right
        Point2d p2 = pt1; p2.y += 15;
        putText(frame, uuid.c_str(), pt1, FONT_HERSHEY_DUPLEX, font_scale,
                Scalar(200, 200, 200), 1);
        putText(frame, description.c_str(), p2, FONT_HERSHEY_DUPLEX, font_scale,
                Scalar(200, 200, 200), 1);
        rectangle(frame, bbox, color, thickness_box, 1);
    }
}