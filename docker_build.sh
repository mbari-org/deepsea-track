#! /bin/sh
# if building for multiple platforms
#docker buildx build --no-cache --platform linux/amd64,linux/arm64 --tag mbari/deepsea-track:latest .
docker build --tag mbari/deepsea-track:latest .
