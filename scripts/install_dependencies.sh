#! /bin/sh

printf "Installing dependencies for PosturePerfection\n"

PP_ROOT=$(pwd)
MIN_CMAKE="3.16"
TF_VERSION="f32b4d37cc059771198484d453d5cf288cf7803b"

## Check for correct version of CMake
if ! cmake --version >/dev/null 2>&1
then
    printf "CMake is not installed, please install version 3.16 or higher\n"
    exit 1
fi

CMAKE_VERSION=$(echo "$(cmake --version)" | awk -F'[. ]' '/[0-9]*\.[0-9]*\.[0-9]*$/ {print $3"."$4}')

if [ ! "$(printf '%s\n' "$MIN_CMAKE" "$CMAKE_VERSION" | sort -V | head -n1)" = "$MIN_CMAKE" ]; then 
      printf "CMake version too low; $CMAKE_VERSION found and ${MIN_CMAKE} needed\n"
      exit 1
fi


## Download TFL code
printf "Downloading TensorFlow ...\n"
cd ..
if [ ! -d "tensorflow_src" ]; then
  git clone https://github.com/tensorflow/tensorflow.git tensorflow_src || exit 1
fi
cd tensorflow_src
git fetch
git checkout $TF_VERSION
cd ..

## Download OpenCV code
printf "Downloading OpenCV ...\n"
if [ ! -d "opencv_src" ]; then
  git clone https://github.com/opencv/opencv.git opencv_src || exit 1
fi

## Build OpenCV
mkdir -p opencv_build
cd opencv_build
cmake -DCMAKE_BUILD_TYPE=Release ../opencv_src || exit 1
make || exit 1

# Install QT5 and libcustoplot for the User Interface
printf "Installing qt5-default and libqcustomplot-dev\n"
sudo apt install -y qt5-default libqcustomplot-dev

## For unit testing
printf "Installing unit testing dependencies\n"
sudo apt install libboost-dev libboost-all-dev -y

## Reset directory to project root
cd $PP_ROOT

printf "\nSuccessfully installed dependencies\n"
