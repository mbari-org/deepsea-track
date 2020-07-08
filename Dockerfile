FROM zouzias/boost:1.73.0
# ================================================================
# Docker Image for deepsea-track
# See https://bitbucket.org/mbari/deepsea-track/src/master/
# ================================================================
# Get base image, which has boost installed by default.

MAINTAINER Peyton Lee <plee@mbari.org>

# Get dependencies
RUN apt-get update --fix-missing && \
    apt-get install -y build-essential cmake git pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev curl

RUN apt-get update && \
    apt-get install -y libboost-all-dev

# Install CMake
RUN curl -L https://github.com/Kitware/CMake/releases/download/v3.17.3/cmake-3.17.3-Linux-x86_64.sh \
    --output /opt/cmake-3.17.3-Linux-x86_64.sh && \
    chmod +x /opt/cmake-3.17.3-Linux-x86_64.sh && \ 
    /opt/cmake-3.17.3-Linux-x86_64.sh --skip-license && \
    rm -f /bin/cmake && \
    ln -s /opt/bin/cmake /bin/cmake && \
    cmake --version \
    rm /opt/cmake-3.17.3-Linux-x86_64.sh \

# ================================================================
# Download OpenCV and build for source using CMake.
# https://linuxize.com/post/how-to-install-opencv-on-ubuntu-18-04/
# ================================================================
# Clone repositories
RUN cd /opt/ && \
    git clone https://github.com/opencv/opencv.git && \
    git clone https://github.com/opencv/opencv_contrib.git

# Create the temporary build directory and build using CMake.
RUN cd /opt/opencv && \
    mkdir build && \
    cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D WITH_TBB=ON \
    -D WITH_OPENMP=ON \
    -D WITH_IPP=ON \
    -D CMAKE_INSTALL_PREFIX=/usr/ \
    -D BUILD_EXAMPLES=OFF \
    -D BUILD_DOCS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D BUILD_TESTS=OFF \
    -D OPENCV_EXTRA_MODULES_PATH=/opt/opencv_contrib/modules \
    /opt/opencv/ && \
    make -j4 && \
    make install    

# Install Xerces-C
RUN cd /opt/ && \
    curl -L https://downloads.apache.org//xerces/c/3/sources/xerces-c-3.2.3.tar.gz \
    --output /opt/xerces-c-3.2.3.tar.gz && \
    tar -xvzf xerces-c-3.2.3.tar.gz && \
    cd xerces-c-3.2.3 && \
    cmake ./ && \
    make && \
    make install
    

# ================================================================
# Download and build the deepsea-track repository
# ================================================================
# Get the deepsea-track directory
#RUN cd /home/ && \
#    git clone https://plee-mbari@bitbucket.org/mbari/deepsea-track.git && \
#    cd deepsea-track

# Create a thirdparty directory and add the nlohmann-json library
#RUN mkdir thirdparty && \
#    git clone https://github.com/nlohmann/json ./thirdparty

# Ready to run CMake!
#RUN /opt/bin/cmake ./ && \
#    make


