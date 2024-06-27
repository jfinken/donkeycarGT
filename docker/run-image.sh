#!/bin/bash

# permit docker to make a connection to your X server (could be placed
#   in your ~/.bash_aliases, e.g.)
xhost +local:docker

XSOCK=/tmp/.X11-unix
GPU_ARGS='--gpus 'all,\"capabilities=compute,utility,display\"''

LOCAL_SOURCE=/home/jfinken/projects/40-49-autonomy/41-sensing-and-perception/41.12-donkeycar/donkeycarGT
REMOTE_SOURCE=/root/donkeycarGT

DK_TUB_IMAGE_PATH="/home/jfinken/projects/40-49-autonomy/41-sensing-and-perception/41.12-donkeycar/mycar/data/murmurpi4_circuit_launch_20210716_1826/data/images"
LOCAL_TUB=${DK_TUB_IMAGE_PATH}
REMOTE_TUB=/root/mycar/data/murmurpi4_circuit_launch_20210716_1826/data/images

docker run -it --privileged \
    -e DISPLAY=$DISPLAY -v $XSOCK:$XSOCK ${GPU_ARGS} \
    -v $HOME/.Xauthority:/root/.Xauthority \
    --net=host \
    -v ${LOCAL_SOURCE}:${REMOTE_SOURCE} \
    -v ${LOCAL_TUB}:${REMOTE_TUB} \
    -e DK_TUB_IMAGE_PATH=${REMOTE_TUB} \
    --name jfinken_donkeycarGT \
    donkeycargt:devel-opencv-4.6.0-ubuntu-20.04
