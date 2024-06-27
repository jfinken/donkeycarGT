#!/bin/bash
set -e

# This is a shell script that configures the build arguements to the Dockefile
# and builds a Docker image with a default tag.    
#              			  
# Available versions to configure the arguements:
# UBUNTU_RELEASE_YEAR = 18 and 20 
# OPENCV_VERSION : Refer to change log https://github.com/opencv/opencv/wiki/ChangeLog#version455 
# to choose your desired version

#Below are th edefault values

UBUNTU_RELEASE_YEAR=20 		#Specify the Ubunutu release year
OPENCV_VERSION=4.6.0 		# OpenCV version


#Check for the version compatibilities

if [ ${UBUNTU_RELEASE_YEAR} == "18" ] ; then
echo "Ubuntu 18.04"
elif [ ${UBUNTU_RELEASE_YEAR} == "20" ] ; then
echo "Ubunutu 20.04"
else
	echo "UBUNTU_RELEASE_YEAR! Allowed values are 18 or 20 "
	exit
fi 

# Default Tag based on the selected versions
TAG="donkeycargt:devel-opencv-${OPENCV_VERSION}-ubuntu-${UBUNTU_RELEASE_YEAR}.04"
           
echo "Building '${TAG}'" 

docker build \
    --build-arg UBUNTU_RELEASE_YEAR=${UBUNTU_RELEASE_YEAR} \
    --build-arg OPENCV_VERSION=${OPENCV_VERSION} \
    -t "${TAG}" -f Dockerfile.donkeycarGT .

