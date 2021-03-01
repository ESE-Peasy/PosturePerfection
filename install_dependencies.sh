#! /bin/sh

echo "Installing dependencies for PosturePerfection"

PP_ROOT=$(pwd)
MIN_CMAKE="3.16"
TF_VERSION="f32b4d37cc059771198484d453d5cf288cf7803b"

while getopts ":t" opt; do
  case ${opt} in
    t) TESTING=1
  esac
done

## Check for correct version of CMake
if ! cmake --version >/dev/null 2>&1
then
    echo "CMake is not installed, please install version 3.16 or higher"
    exit 1
fi

CMAKE_VERSION=$(echo "$(cmake --version)" | awk -F'[. ]' '/[0-9]*\.[0-9]*\.[0-9]*$/ {print $3"."$4}')

if [ ! "$(printf '%s\n' "$MIN_CMAKE" "$CMAKE_VERSION" | sort -V | head -n1)" = "$MIN_CMAKE" ]; then 
      echo "CMake version too low; $CMAKE_VERSION found and ${MIN_CMAKE} needed"
      exit 1
fi


## Download TFL code
echo "Downloading TensorFlow ..."
cd ..
if [ ! -d "tensorflow_src" ]; then
  git clone https://github.com/tensorflow/tensorflow.git tensorflow_src || exit 1
fi
cd tensorflow_src
git fetch
git checkout $TF_VERSION
cd ..

## Download OpenCV code
echo "Downloading OpenCV ..."
if [ ! -d "opencv_src" ]; then
  git clone https://github.com/opencv/opencv.git opencv_src || exit 1
fi

## Build OpenCV
mkdir -p opencv_build
cd opencv_build
cmake -DCMAKE_BUILD_TYPE=Release ../opencv_src || exit 1
make || exit 1

## Download OpenCV code
cd ..
echo "Downloading OpenCV ..."
if [ ! -d "opencv_src" ]; then
  git clone https://github.com/opencv/opencv.git opencv_src
fi

## Build OpenCV
mkdir -p opencv_build
cd opencv_build
cmake -DCMAKE_BUILD_TYPE=Release ../opencv_src
make -j7

## Reset directory to project root
cd $PP_ROOT

echo ""
echo "Successfully installed dependencies"

if [ $TESTING ]
then
  ## For unit testing
  sudo apt install libboost-dev libboost-all-dev -y
  echo "Installed unit testing dependencies"
fi
