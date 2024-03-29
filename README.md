[![MBARI](https://www.mbari.org/wp-content/uploads/2014/11/logo-mbari-3b.png)](http://www.mbari.org)

[![semantic-release](https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg)](https://github.com/semantic-release/semantic-release)
![Supported Platforms](https://img.shields.io/badge/Supported%20Platforms-Windows%20%7C%20macOS%20%7C%20Linux-green)
![license-GPL](https://img.shields.io/badge/license-GPL-blue)

# About

*deepsea-track* is online tracking software for tracking multiple objects in deep sea underwater video.
It is designed to be used with output from object detection models sent over [ZeroMQ](https://zeromq.org/)
 or in data formatted into XML files in [PASCAL VOC](http://host.robots.ox.ac.uk/pascal/VOC/) format. 
It generates unique *VisualEvent* track sequences in JSON format.

---

[click image below to see example in YouTube]
[![Example video output](https://img.youtube.com/vi/cMZ8vr0aAYI/maxresdefault.jpg)](https://youtu.be/cMZ8vr0aAYI)

# Questions?

If you have any questions or are interested in contributing, please contact me at dcline@mbari.org.

*Danelle Cline*
https://www.mbari.org/cline-danelle-e/

---

# Requirements
- [Docker](www.docker.com)

Alternatively, can be built natively for Mac with

- A compiler that support >= C++11
- [CMake `>= 3.15`](https://cmake.org/download/)
- [HomeBrew](https://brew.sh/) for Mac OS install

---

## How to use deepsea-track

* Create a folder, e.g. "benthic" and add your video, class map, configuration file and folder
 to store the results, e.g.:
        
~~~
│   └── benthic
│       ├── video.mp4
│       ├── deepsea_class_map.json
│       ├── deepsea_cfg.json
│       ├── f001000.json
│       ├── f001001.json
│       ├── f001002.json
│       ...
│       ├── f001010.json
│   └── benthic_tracks
~~~

### Run in Docker

The easiest way to run it is in a docker image. See https://hub.docker.com/u/mbari/deepsea-track for available releases.

To get a list of arguments:

```
docker run mbari/deepsea-track --help
```

## *Arguments* 

                        
  * --video_path absolute path to the video file to process, e.g. /data/video.mp4, /data/video.mov
  * --config_path absolute path to the configuration files: deepsea_class_map.json and deepsea_cfg.json
  * --out_path absolute path to save output artifacts
  * --xml_path (optional) absolute path to directory with voc xml files. If absent, --address and --topic must be set
  * --address (optional) socket address for the detector output, e.g. tcp://127.0.0.1:6432
  * --topic (optional) topic to listen on at address, e.g. VisualEvents. If specified, must also specify the --address 
  * --tracker_width resize width in pixels for running the tracker, defaults to 512
  * --tracker_height resize height in pixels for running the tracker, defaults to 512 
  * --out_width width in pixels to scale the output to, defaults to 1920
  * --out_height height in pixels to scale the output to, defaults to 1080
  * --start_frame_num (optional) starting frame to process, 1-based. e.g. --start_frame=1 is the first frame in the video. Defaults to 1.
  * --stride (optional) amount to stride seeding new detection between frames. A larger stride may process faster. Defaults to 1.
  

## Example

Docker commands:

 * -it = run interactively
 * --rm = remove after execution
 * -v $PWD:/data = mount the current working directory to /data in the container

e.g.
- process video file /data/benthic/video.mp4
- configuration files in /data/benthic
- output results to mapped /data mount in the directory /data/benthic_tracks
- *no start frame specified - start at frame 1*
- *no stride specified - process every frame; only strides detection input*
- *no tracker_width specified - default to 512*
- *no tracker_height specified - default to 512*
- *no out_width specified - default to 1920*
- *no out_height specified - default to 1080*


```
docker run -it --rm -v $PWD:/data mbari/deepsea-track --video_path /data/benthic/video.mp4 --cfg_path /data/benthic/ --xml_path /data/benthic --out_path /data/benthic_tracks/
```

The output will look like:
           
~~~ 
│   └── benthic_tracks
│       ├── f000001.json
│       ├── f000002.json
│       ├── f000003.json
│       ├── f000004.json
│       ...    
│       ├── results.mp4    
~~~

---

## JSON output schema

Data is output per each frame with all events tracked per the following schema:

```json
{
   "$schema": "http://json-schema.org/draft-04/schema#",
   "title": "deepsea-track",
   "description": "a collection of visual events",
   "type": "object",
	
   "properties": {
	
      "uuid": {
         "description": "The unique identifier for this VisualEvent",
         "type": "string"
      },
		
      "bounding_box": {
         "description": "Bounding Box",
         "type": "object",
         "properties": {
            "x": "top left x coordinate",
            "y": "top left y coordinate",
            "width": "The width in pixels from top left",
            "height": "The height in pixels from top left"
        }        
      },
		
      "occlusion": {
         "description": "Occlusion - an approximation based on intersection over union",
         "type": "integer"
      },

      "class_name": {
         "description": "The unique class name ",
         "type": "string"
      },

      "class_index": {
         "description": "The unique class index",
         "type": "integer"
      },

      "confidence": {
         "description": "Confidence score for a given class_name/index between 0-100",
         "type": "integer"
      },

      "surprise": {
         "description": "Surprise factor 0-TBD",
         "type": "integer"
      }, 
   },
	
   "required": ["uuid", "bounding_box", "occlusion",  "class_name", "class_index", "confidence", "surprise"]
}
```

---

### Configuration files

The *deep_class_map.json* should contain the mapping from your IDs to actual class names.
This is human-readable descriptions and colors to associate to each class, e.g.
```json
{
    "items":[
      {
        "id": 1,
        "map": [{"name": "PENIAGONE_VITRAE", "description": "Peniagone vitrea", "color": "89  233 206"}]
      },
      {
        "id": 2,
        "map": [{"name": "PENIAGONE_SP_A", "description": "Peniagone sp. A", "color": "211  199 0"}]
      }
    ]
}
```

The *deepsea_cfg.json* should contain the index for the type of tracker you'd like to use:
These correspond to a few available [OpenCV trackers](https://docs.opencv.org/). 

| id   | Tracker           |  Comments |
|----------|---------------|--------------|
| -1 | No tracker  |  |
| 0 | CSRT Discriminative Correlation Filter with Channel and Spatial Reliability  | Slower, but better performance than KCF |
| 1 | KCF Kernelized Correlation Filter  | Fast, but does not handle scale changes and unpredictable movement well |

e.g. this uses the CSRT tracker:
*deepsea_cfg.json*
```json
{
  "tracker": 0,
  "create_video": true,
  "min_event_frames": 3,
  "display": false,
  "nms_threshold": 0.5,
  "score_threshold": 0.4,
  "display_wait_msecs": 2000,
  "tracker_stride": 2,
  "gamma_enhance": false
}
```

| field   | description           |
|----------|---------------|
| tracker | The tracker id - must be a valid id  |
| tracker_stride | Run the tracker every 2 frames instead of every frame for speed-up |
| gamma_enhance |  Gamma enhance the input video. Brightens dark videos; caution - this add processing time|
| tracker_wait_msecs |  The time in milliseconds to pause between running the tracker on each frame; useful if detector messages are slow over zmq |
| create_video | Set *true* to create a .mp4 video of the output; caution - this adds processing time |
| display | Set *true* to see the frame output displayed while running |
| display_wait_msecs | The time in milliseconds to wait to while displaying the frame output. Useful for debugging. |
| min_event_frames | Only VisualEvents greater than this number of frames are stored; used to remove short invalid tracks. |
| nms_threshold | The minimum score for the non maximum supression algorithm.  If nms_threshold is set too low, e.g. 0.1, it will not detect overlapping objects of same or different classes; set too high e.g. 1, will return multiple boxes for the same object. |
| score_threshold | The minimum score for detections. Set this low to allow more weak detections. |

# Building

An image is available on hub.docker.com at [mbari/deepsea-track](https://hub.docker.com/r/mbari/deepsea-track).
This can be build from source with:

## Docker
```
docker build -t deepsea-track -f Dockerfile.amd .
```

## Mac OSX Native
Note that the CMake files in this project require version 3.15 or higher
```
brew install boost
brew tap nlohmann/json
brew install nlohmann/json
brew install cppzmq
brew install opencv
brew install xerces
```

## Roadmap
-  Add support for inline object detection inference using the [OpenVINO Toolkit](docs.openvinotoolkit.org)
-  Add to docker build 
```
cd ../../thirdparty &&
git clone --recursive https://github.com/Microsoft/onnxruntime && \
    cd ./onnxruntime && build.sh --config RelWithDebInfo --build_shared_lib --parallel && \
    cd ./Linux/RelWithDebInfo && make install lib
```
