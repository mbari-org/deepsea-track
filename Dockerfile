FROM zouzias/boost:1.73.0
# ================================================================
# Docker Image for mbari/deepsea-track
# ================================================================
# Get base image, which has boost installed by default.

MAINTAINER Peyton Lee <plee@mbari.org>, Danelle Cline <dcine@mbari.org>

# Get dependencies
RUN apt-get update --fix-missing && \
    apt-get install -y build-essential cmake git pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev curl libboost-all-dev libzmq3-dev

# Install CMake
RUN curl -L https://github.com/Kitware/CMake/releases/download/v3.17.3/cmake-3.17.3-Linux-x86_64.sh \
    --output /opt/cmake-3.17.3-Linux-x86_64.sh && \
    chmod +x /opt/cmake-3.17.3-Linux-x86_64.sh && \ 
    mkdir /opt/cmake && \
    /opt/cmake-3.17.3-Linux-x86_64.sh --skip-license --prefix=/opt/cmake/ && \
    rm -f /bin/cmake && \
    ln -s /opt/cmake/bin/cmake /bin/cmake && \
    /opt/cmake/bin/cmake --version && \
    cmake --version

WORKDIR /tmp/build

# Download and build OpenCV with contrib modules
RUN curl -L https://github.com/opencv/opencv/archive/4.5.0.tar.gz --output opencv4.5.0.tar.gz && \
    curl -L https://github.com/opencv/opencv_contrib/archive/4.5.0.tar.gz --output opencv-contrib4.5.0.tar.gz && \ 
    tar -xzf opencv4.5.0.tar.gz && tar -xzf opencv-contrib4.5.0.tar.gz && \
    cd opencv-4.5.0 && mkdir build && cd build && \
    cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D WITH_TBB=ON \
    -D WITH_OPENMP=ON \
    -D WITH_IPP=ON \
    -D CMAKE_INSTALL_PREFIX=/usr/ \
    -D BUILD_EXAMPLES=OFF \
    -D BUILD_DOCS=OFF \
    -D BUILD_PERF_TESTS=OFF \
    -D BUILD_TESTS=OFF \
    -D OPENCV_EXTRA_MODULES_PATH=/tmp/build/opencv_contrib-4.5.0/modules \
    /tmp/build/opencv-4.5.0/ && \
    make -j8 && make install

# Download and build Xerces-C for parsing XML
RUN curl -L https://downloads.apache.org//xerces/c/3/sources/xerces-c-3.2.3.tar.gz \
    --output xerces.tar.gz && \
    tar -xzf xerces.tar.gz && \
    cd xerces-c-3.2.3 && \
    cmake ./ && \
    make -j8 &&  make install

RUN curl -L https://github.com/nlohmann/json/archive/v3.9.1.tar.gz --output v3.9.1.tar.gz && \
    tar -xzf v3.9.1.tar.gz && cd json-3.9.1 && \
    cmake . && make -j8 && make install

RUN curl -L https://github.com/zeromq/cppzmq/archive/v4.7.1.tar.gz --output v4.7.1.tar.gz && \
    tar -xzf v4.7.1.tar.gz && cd cppzmq-4.7.1 && \
    cmake -DCPPZMQ_BUILD_TESTS=OFF . && make -j8 && make install

# Build the deepsea-track repository
COPY . /tmp/build 

RUN /opt/cmake/bin/cmake --version && \
    /opt/cmake/bin/cmake ./ && \
    make && cp apps/deepsea-track /usr/local/bin/

# Clean-up
RUN rm -rf /tmp/build

WORKDIR /app

ENTRYPOINT ["/usr/local/bin/deepsea-track"]
