[MBARI](https://www.mbari.org/wp-content/uploads/2014/11/logo-mbari-3b.png)
<p align="center">
    <b> <img src="https://img.shields.io/badge/Supported%20Platforms-Windows%20%7C%20macOS%20%7C%20Linux-green" title="Supported Platforms"/> </b> <br>
    <b> <img src="https://img.shields.io/badge/license-GPL-blue" title="license-GPL"/> </b> <br>
 <a href="https://codecov.io/gh/semantic-release/semantic-release">
    <img alt="Codecov" src="https://img.shields.io/codecov/c/github/semantic-release/semantic-release/master.svg">
  </a>
</p>

# About

*deepsea-track* is lightweight tracking software for tracking multiple objects in deep sea underwater video.
It is designed to be used with output from object detection from [deepsea-tfdetect](https://github.com/mbari-org/deepsea-tfdetect)
which generate collections of XML files in [PASCAL VOC](http://host.robots.ox.ac.uk/pascal/VOC/) format but 
can be used with any output generated in XML format. It generates unique *VisualEvent* track sequences for analysis.

[![Example video output](https://img.youtube.com/vi/cMZ8vr0aAYI/maxresdefault.jpg)](https://youtu.be/cMZ8vr0aAYI)

# Questions?

If you have any questions, or are interested in helping with the development, please contact me at: dcline@mbari.org

*Danelle Cline*
https://www.mbari.org/cline-danelle-e/

---

# Requirements
- A compiler that support >= C++11
- [CMake `>= 3.1`](https://cmake.org/download/)
- (optional) [Docker](www.docker.com)
- [HomeBrew](https://brew.sh/) for Mac OS install

---

### How to use deepsea-track

* Create a folder, e.g. "benthic" below and add your video, class map, configuration file and location
 for the results, e.g.:
        
    ```bash
    Users
    ├── yogi
    │   └── benthic
    │       ├── video.mp4
    │       ├── deepsea_class_map.json
    │       ├── deepsea_cfg.json
    │   └── benthic_results
  
    ```

* Run with 

```
deepsea-track <video_name> <path to xml> <start frame num> <frame resize ratio> <stride(optional)>
```

e.g.

```
deepsea-track  /Users/yogi/benthic/video.mp4  /Users/yogi/benthic_results 1 0.5
```

Frames and output will be rescaled by 0.5 in width and height in the above example.
Output will look like:
      
    ```bash
    Users
    ├── yogi
    │   └── benthic_results
    │       ├── f000001.json
    │       ├── f000002.json
    │       ├── f000003.json
    │       ├── f000004.json
    │       ...    
    │       ├── results.mp4
  
    ```

TODO: add details on .json output

---

### Configuration files

* The *deep_class_map.json* should contain the mapping from your IDs to actual class names,
 human-readable descriptions and colors to associate to each class, e.g.
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

* The *deepsea_cfg.json* should contain the index for the type of tracker you'd like to use:
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

| field   | description           |
|----------|---------------|
| tracker1 | The primary tracker - must be a valid id  |
| tracker2 | Optional secondary tracker - can be -1 or No tracker  |

### Installation

TODO: refactor third-party build into CMake

## Mac OSX 
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

## Windows
```
TODO: add instructions here
```

## Linux Debian or Ubuntu
```
sudo apt-get update
sudo apt-get install build-essential 
...
```

## Docker
```
TODO: add docker build
```


