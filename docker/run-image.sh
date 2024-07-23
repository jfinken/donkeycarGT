#!/bin/bash

# permit docker to make a connection to your X server (could be placed
#   in your ~/.bash_aliases, e.g.)
xhost +local:docker

XSOCK=/tmp/.X11-unix
GPU_ARGS='--gpus 'all,\"capabilities=compute,utility,display\"''

# Paths and sources mounted within the container
LOCAL_SOURCE=/home/jfinken/projects/40-49-autonomy/41-sensing-and-perception/41.12-donkeycar/donkeycarGT
REMOTE_SOURCE=/root/donkeycarGT

LOCAL_MYCAR=/home/jfinken/projects/40-49-autonomy/41-sensing-and-perception/41.12-donkeycar/mycar/
REMOTE_MYCAR=/root/mycar/
REMOTE_TUB=${REMOTE_MYCAR}/data/murmurpi4_circuit_launch_20210716_1826/data/images

# TODO: build TF into the image itself
TFLITE_LOCAL_SOURCE=/home/jfinken/projects/40-49-autonomy/41-sensing-and-perception/41.14-tensorflow/tensorflow
TFLITE_REMOTE_SOURCE=/root/tensorflow

docker run -it --privileged \
    -e DISPLAY=$DISPLAY -v $XSOCK:$XSOCK ${GPU_ARGS} \
    -v $HOME/.Xauthority:/root/.Xauthority \
    --net=host \
    -v ${LOCAL_SOURCE}:${REMOTE_SOURCE} \
    -v ${LOCAL_MYCAR}:${REMOTE_MYCAR} \
    -v ${TFLITE_LOCAL_SOURCE}:${TFLITE_REMOTE_SOURCE} \
    -e DK_TUB_IMAGE_PATH=${REMOTE_TUB} \
    --name jfinken_donkeycarGT \
    donkeycargt:devel-opencv-4.6.0-ubuntu-20.04
