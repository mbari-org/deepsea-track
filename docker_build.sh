#! /bin/sh
docker buildx use deepsea-track-docker-builder

echo "Building deepsea-track for arm64, cuda 11.3.1, ubuntu20.04"
docker buildx bake -f docker-compose.yml --load --progress=plain arm64
docker image push mbari/deepsea-track:ubuntu20.04-cuda11.3.1-arm64

echo "Building deepsea-track for amd64, cuda 11.3.1, ubuntu20.04"
docker buildx bake -f docker-compose.yml --load --progress=plain amd64
docker image push mbari/deepsea-track:ubuntu20.04-cuda11.3.1-amd64
