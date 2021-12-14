#! /bin/sh
VERSION=`cat VERSION.txt`

docker buildx use deepsea-track-docker-builder

echo "Building deepsea-track for arm64, cuda 11.3.1, ubuntu20.04"
#docker buildx bake -f docker-compose.yml --load --progress=plain arm64
#docker image push mbari/deepsea-track:ubuntu20.04-cuda11.3.1-arm64
#docker tag mbari/deepsea-track:ubuntu20.04-cuda11.3.1-arm64
#docker push mbari/deepsea-track:ubuntu20.04-cuda11.3.1-arm64-v${VERSION}

echo "Building deepsea-track for amd64, cuda 11.3.1, ubuntu20.04"
docker buildx bake -f docker-compose.yml --load --progress=plain amd64
docker image push mbari/deepsea-track:ubuntu20.04-cuda11.3.1-amd64
docker tag mbari/deepsea-track:ubuntu20.04-cuda11.3.1-amd64 mbari/deepsea-track:ubuntu20.04-cuda11.3.1-amd64-v${VERSION}
docker push mbari/deepsea-track:ubuntu20.04-cuda11.3.1-amd64-v${VERSION}
