#include "event_object.h"

namespace deepsea {

// ######################################################################
// ###### event_object
// ######################################################################
    EventObject::EventObject()
            : bbox_tracker_(Rect(0, 0, 0, 0)),
              occlusion(0.),
              frame_num_(0),
              class_name_("Unknown"),
              class_confidence_(-1.),
              class_index_(0),
              surprise_(0.){}

// ######################################################################
    EventObject::EventObject(const VOCObject &voc, const unsigned int occlusion, const unsigned int frame_num)
            : bbox_tracker_(voc.getBox()),
              occlusion(occlusion),
              frame_num_(frame_num),
              class_name_(voc.getName()),
              class_confidence_(voc.getScore()),
              class_index_(voc.getIndex()),
              surprise_(0.){
    }

// ######################################################################
    EventObject &EventObject::operator=(const EventObject &object) {
        this->bbox_tracker_ = object.bbox_tracker_;
        this->occlusion = object.occlusion;
        this->mask_ = object.mask_;
        this->class_confidence_ = object.class_confidence_;
        this->class_name_ = object.class_name_;
        this->frame_num_ = object.frame_num_;
        this->class_index_ = object.class_index_;
        this->uuid_ = object.uuid_;
        this->surprise_ = object.surprise_;
        return *this;
    }

// ######################################################################
    unsigned int EventObject::getArea() const {
        return (bbox_tracker_.height * bbox_tracker_.width);
    }

}