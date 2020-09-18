[![MBARI](https://www.mbari.org/wp-content/uploads/2014/11/logo-mbari-3b.png)](http://www.mbari.org)

[![semantic-release](https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg)](https://github.com/semantic-release/semantic-release)
![Supported Platforms](https://img.shields.io/badge/Supported%20Platforms-Windows%20%7C%20macOS%20%7C%20Linux-green)
![license-GPL](https://img.shields.io/badge/license-GPL-blue)

# About

*deepsea-track* is lightweight tracking software for tracking multiple objects in deep sea underwater video.
It is designed to be used with output from object detection models in data formatted into XML files in [PASCAL VOC](http://host.robots.ox.ac.uk/pascal/VOC/) format but 
can be used with any output generated in XML format. It generates unique *VisualEvent* track sequences for analysis in JSON format.

[click image below to see example in YouTube]
[![Example video output](https://img.youtube.com/vi/cMZ8vr0aAYI/maxresdefault.jpg)](https://youtu.be/cMZ8vr0aAYI)

# Questions?

If you have any questions, or are interested in helping with the development, please contact me at: dcline@mbari.org

*Danelle Cline*
https://www.mbari.org/cline-danelle-e/

---

# Requirements
- [Docker](www.docker.com)

Alternatively, can be built natively for Mac with

- A compiler that support >= C++11
- [CMake `>= 3.1`](https://cmake.org/download/)
- [HomeBrew](https://brew.sh/) for Mac OS install

---

## How to use deepsea-track

* Create a folder, e.g. "benthic" and add your video, class map, configuration file and folder
 to store the results, e.g.:
        
~~~
│   └── benthic
│       ├── video.mp4
│       ├── deepsea_class_map.json
│       ├── f001000.json
│       ├── f001001.json
│       ├── f001002.json
│       ...
│       ├── f001010.json
│   └── benthic_results
~~~

## Roadmap
-  Add support for object detection inference using the [OpenVINO Toolkit](docs.openvinotoolkit.org)

### Docker

A docker image is available in dockerhub.com at https://hub.docker.com/u/mbari/deepsea-track

Run with 

- path to video/xml - both must be in the same directory
- path to store results
- start frame - 6-digit frame prefix to start, by default will process until the end of of the xml sequence.
- frame resize ratio ratio between 0-1.0 to resize the input video. Smaller resize ratio will process faster.
- stride(optional) - amount to stride between frames. Default is 1. Larger stride will process faster.
```
docker run -it --rm -v $PWD:/data mbari/deepsea-track <path to video/xml> <path to store results> <start frame num> <frame resize ratio> <stride(optional)>
```

e.g.

```
docker run -it --rm -v $PWD:/data mbari/deepsea-track  /data/benthic/video.mp4  /data/benthic_results 1 0.5
```

Frames and output will be rescaled by 0.5 in width and height in the above example.
Output will look like:
           
~~~ 
│   └── benthic_results
│       ├── f000001.json
│       ├── f000002.json
│       ├── f000003.json
│       ├── f000004.json
│       ...    
│       ├── results.mp4    
~~~

TODO: add details on .json output

---

### Configuration files

The *deep_class_map.json* should contain the mapping from your IDs to actual class names.
This is human-readable descriptions and colors to associate to each class, e.g.
```
{
    "items":[
      {
        "id": 1,
        "map": [{"name": "PENIAGONE_VITRAE", "description": "Peniagone vitrae", "color": "89  233 206"}]
      },
      {
        "id": 2,
        "map": [{"name": "PENIAGONE_SP_A", "description": "Peniagone sp. A", "color": "211  199 0"}]
      }
    ]
}
```

The *deepsea_cfg.json* should contain the index for the type of tracker you'd like to use:
These correspond to 4 of the 8 available [OpenCV](https://docs.opencv.org/) trackers.

| id   | Tracker           |
|----------|---------------|
| -1 | No tracker  |
| 0 | MEDIANFLOW  |
| 1 | KCF Kernelized Correlation Filter  |
| 2 | TLD Tracking, learning and detection  |
| 3 | MOSSE Minimum Output Sum of Squared Error  |
| 4 | CSRT Discriminative Correlation Filter with Channel and Spatial Reliability  |

e.g. this uses the MEDIANFLOW tracker and drops the second tracker:
*deepsea_cfg.json*
```
{
  "tracker1": 0,
  "tracker2": -1,
  "min_event_frames": 3
}
```
e.g. this uses a combined MEDIANFLOW and KCF tracker:
*deepsea_cfg.json*
```
{
  "tracker1": 0,
  "tracker2": 1,
  "min_event_frames": 3
}
```

| field   | description           |
|----------|---------------|
| tracker1 | The primary tracker - must be a valid id  |
| tracker2 | Optional secondary tracker - can be -1 or No tracker  |

# Building

## Docker
```
docker build -t deepsea-track .
```

## Mac OSX Native
```
export APP_HOME=$PWD
brew install boost
brew install nlohmann-json
curl -O https://downloads.apache.org/xerces/c/3/sources/xerces-c-3.2.2.tar.gz
tar -zxvf xerces-c-3.2.2.tar.gz
mkdir ./lib/xerces-c-3.2.2/build && cd ./lib/xerces-c-3.2.2/build
cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=$APP_HOME/thirdparty/xerces-c -DCMAKE_BUILD_TYPE=Debug -Dmessage-loader=icu $APP_HOME/lib/xerces-c-3.2.2/
make -j8 && make test && make install
```
