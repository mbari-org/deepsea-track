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
///\file VOCUtils.H methods to parse PASCAL VOC formatted .xml files.

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <list>  // Change into a diff DS or make our own class
#include <string>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "voc_object.h"
#include "config_class_map.h"

using namespace std;
using namespace xercesc;

namespace deepsea {

    struct Box {
        int xmin;
        int ymin;
        int xmax;
        int ymax;
    };


// 	- stores value in temporary 'BoundingBox' voc_object
    void storeValue(Box &temp, const string &tagName,
                    const string &tagValue) {    // Stores value in temporary 'voc_object' voc_object
        // convert Bounding Box Value from 'string to int'
        istringstream is(tagValue);
        int dim = 0;
        is >> dim;

        if (tagName == "xmin") {
            temp.xmin = dim;
        } else if (tagName == "ymin") {
            temp.ymin = dim;
        } else if (tagName == "xmax") {
            temp.xmax = dim;
        } else if (tagName == "ymax") {
            temp.ymax = dim;
        }
    }

// reads .xml for values in <object> ... </object>
    void getObjectValues(XercesDOMParser *itsParser, list<VOCObject> &obj_list, ConfigMaps &cfg) {
        DOMNodeList *list = NULL;
        DOMDocument *domDocParser = itsParser->getDocument();

        // Hhow many instances of the '<tag>' found
        XMLCh *source = XMLString::transcode("object");        // Tag wanted
        list = domDocParser->getElementsByTagName(source);        // Returns list of '<tag>' found

        // parse through each object to grab values
        for (int i = 0; i < list->getLength(); ++i) {

            DOMNode *node = list->item(i);                        // Gets the ith <object> in the list
            DOMNodeList *length = node->getChildNodes();        // Lines counted, including: "<object> ... </object>" = 13 lines total

            Box tempDim;
            VOCObject tmp_object;
            Rect invalid_rect;
            tmp_object.setBox(invalid_rect);

            // iterate through each <tag> in <object> ... </object> to retrieve values
            for (int k = 0; k < length->getLength(); ++k) {
                DOMNode *childNode = length->item(k);

                if (childNode->getNodeType() ==
                    DOMNode::ELEMENT_NODE) {                        // Ensures we found a <tag>
                    string tag_name = XMLString::transcode(childNode->getNodeName());        // <Gets Tag Name>
                    string tag_value = XMLString::transcode(childNode->getTextContent());    // <tag> Gets Value </tag>
                    tmp_object.setConfidence(1.0); // set confidence to 1.0 as default

                    // grab bounding box dimensions, otherwise, get the <name>
                    if (tag_name == "bndbox") {
                        DOMNodeList *dimensions = childNode->getChildNodes();                        // Gets all the <tags> in <bndbox>

                        for (int j = 0; j <
                                        dimensions->getLength(); ++j) {                            // Iterate each for dim. value
                            DOMNode *dim = dimensions->item(j);

                            if (dim->getNodeType() == DOMNode::ELEMENT_NODE) {
                                string tagNameBB = XMLString::transcode(dim->getNodeName());        // <Gets Tag Name>
                                string tagValueBB = XMLString::transcode(
                                        dim->getTextContent());    // <tag> Gets Value </tag>
                                storeValue(tempDim, tagNameBB,
                                           tagValueBB);                            // Store the dim values 1 by 1
                            }
                        }

                        int ymin = int(tempDim.ymin);
                        int ymax = int(tempDim.ymax);
                        int xmin = int(tempDim.xmin);
                        int xmax = int(tempDim.xmax);

                        Rect tmp_box = Rect(xmin, ymin, xmax - xmin, ymax - ymin);
                        tmp_object.setBox(tmp_box);
                        break;
                    } else if (tag_name == "name") {
                        tmp_object.setName(tag_value);
                        tmp_object.setIndex(cfg.class_ids[tag_value]);
                    } else if (tag_name == "confidence") {
                        // convert Confidence value from 'string to double'
                        istringstream is(tag_value);
                        double con = 0;
                        is >> con;
                        tmp_object.setConfidence(con);
                    }

                }
            }

            //tmp_object.printDescription();
            obj_list.push_back(tmp_object);
        }
        cout << "VOC Object List Size: " << obj_list.size() << endl;
    }

}
