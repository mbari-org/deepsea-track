# ================================================================
# Docker Image for deepsea-track
# See https://bitbucket.org/mbari/deepsea-track/src/master/
# ================================================================
# Get base image, which has boost installed by default.
FROM zouzias/boost:1.73.0

MAINTAINER Peyton Lee <plee@mbari.org>

# Install CMake
RUN apt-get -y install curl
RUN curl -L https://github.com/Kitware/CMake/releases/download/v3.17.3/cmake-3.17.3-Linux-x86_64.sh --output /opt/cmake-3.17.3-Linux-x86_64.sh
RUN chmod +x /opt/cmake-3.17.3-Linux-x86_64.sh && /opt/cmake-3.17.3-Linux-x86_64.sh --skip-license && ln -s /opt/bin/cmake /usr/bin

# ================================================================
# Download OpenCV and build for source using CMake.
# https://linuxize.com/post/how-to-install-opencv-on-ubuntu-18-04/
# ================================================================
# Get dependencies
RUN apt-get install -y build-essential cmake git pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev

# Clone repositories
RUN cd /opt/ && git clone https://github.com/opencv/opencv.git
RUN git clone https://github.com/opencv/opencv_contrib.git

# Create the temporary build directory and build using CMake.
RUN cd /opt/opencv && mkdir build && cd build 
RUN cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/ \
    -D OPENCV_GENERATE_PKGCONFIG=ON \
    -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ..
RUN make install

# Add missing boost libraries?
RUN apt-get install -y libboost-all-dev

# ================================================================
# Download and build the deepsea-track repository
# ================================================================
# Get the deepsea-track directory
RUN cd /home/ && git clone https://plee-mbari@bitbucket.org/mbari/deepsea-track.git && cd deepsea-track

# Create a thirdparty directory and add the nlohmann-json library
RUN mkdir thirdparty
RUN git clone https://github.com/nlohmann/json ./thirdparty

# Ready to run CMake!
RUN cmake -DOpenCV_INCLUDE_DIR= ./ && make
