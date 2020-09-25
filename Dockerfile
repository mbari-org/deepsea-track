FROM zouzias/boost:1.73.0
# ================================================================
# Docker Image for mbari/deepsea-track
# ================================================================
# Get base image, which has boost installed by default.

MAINTAINER Peyton Lee <plee@mbari.org>, Danelle Cline <dcine@mbari.org>

RUN chmod 1777 /tmp

# Get dependencies
RUN apt-get update --fix-missing && \
    apt-get install -y build-essential cmake git pkg-config libgtk-3-dev \
    libavcodec-dev libavformat-dev libswscale-dev libv4l-dev \
    libxvidcore-dev libx264-dev libjpeg-dev libpng-dev libtiff-dev \
    gfortran openexr libatlas-base-dev python3-dev python3-numpy \
    libtbb2 libtbb-dev libdc1394-22-dev curl libboost-all-dev

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


# Download and build OpenCV with contrib modules
# https://linuxize.com/post/how-to-install-opencv-on-ubuntu-18-04/
# Clone repositories
RUN cd /opt/ && \
    git clone https://github.com/opencv/opencv.git /opt/opencv && \
    git clone https://github.com/opencv/opencv_contrib.git /opt/opencv_contrib

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

# Download and build Xerces-C for parsing XML
RUN cd /opt/ && \
    curl -L https://downloads.apache.org//xerces/c/3/sources/xerces-c-3.2.3.tar.gz \
    --output /opt/xerces-c-3.2.3.tar.gz && \
    tar -xvzf xerces-c-3.2.3.tar.gz && \
    cd xerces-c-3.2.3 && \
    cmake ./ && \
    make && \
    make install 
RUN apt install -y libxerces-c3.2    

# Download and build onnxruntime for executing ONNX models
RUN git clone --recursive https://github.com/Microsoft/onnxruntime && \
    mkdir ./onnxruntime/build && cd ./onnxruntime/build && \
    ./build.sh --config Release --skip_submodule_sync --build_shared_lib --parallel && \
    cd Linux/Release && make install lib

# for CUDA build: --config Release --skip_submodule_sync  --parallel --build_shared_lib --use_cuda --cuda_version=10.2 --cuda_home=/usr/local/cuda-10.2 --cudnn_home=/usr/local/cuda-10.2

# Download and build the deepsea-track repository
COPY . /home/deepsea-track

RUN cd /home/deepsea-track && \
    mkdir thirdparty && \
    git clone https://github.com/nlohmann/json ./thirdparty

RUN cd /home/deepsea-track/ && \
    /opt/cmake/bin/cmake --version && \
    /opt/cmake/bin/cmake ./ && \
    make

CMD /home/deepsea-track/deepsea-track

