#! /bin/sh

printf "Installing dependencies for PosturePerfection\n"

PP_ROOT=$(pwd)
MIN_CMAKE="3.16"
TF_VERSION="f32b4d37cc059771198484d453d5cf288cf7803b"

## Check for correct version of CMake
if ! cmake --version >/dev/null 2>&1; then
  printf "CMake is not installed, please install version 3.16 or higher\n"
  exit 1
fi

CMAKE_VERSION=$(echo "$(cmake --version)" | awk -F'[. ]' '/[0-9]*\.[0-9]*\.[0-9]*$/ {print $3"."$4}')

if [ ! "$(printf '%s\n' "$MIN_CMAKE" "$CMAKE_VERSION" | sort -V | head -n1)" = "$MIN_CMAKE" ]; then
  printf "CMake version too low; $CMAKE_VERSION found and ${MIN_CMAKE} needed\n"
  exit 1
fi

cd ..

## Download cppTimer
printf "Downloading cppTimer ..."
if [ ! -d "cppTimer_src" ]; then
  printf "\n"
  git clone https://github.com/berndporr/cppTimer.git cppTimer_src || exit 1
  cd cppTimer_src
  cmake .
  make
  sudo make install
  cd ..
else
  printf " skipped\n"
fi

## Download TFL code
printf "Downloading TensorFlow ..."
if [ ! -d "tensorflow_src" ]; then
  printf "\n"
  git clone https://github.com/tensorflow/tensorflow.git tensorflow_src || exit 1
  cd tensorflow_src
  git checkout $TF_VERSION
  cd ..
else
  printf " skipped\n"
fi

## Download OpenCV code
printf "Downloading OpenCV ..."
if [ ! -d "opencv_src" ]; then
  printf "\n"
  git clone https://github.com/opencv/opencv.git opencv_src || exit 1
else
  printf " skipped\n"
fi

## Build OpenCV
printf "Building OpenCV ..."
if [ ! -d "opencv_build" ]; then
  printf "\n"
  mkdir -p opencv_build
  cd opencv_build
  cmake -DCMAKE_BUILD_TYPE=Release ../opencv_src || exit 1
  make || exit 1
  cd ..
else
  printf " skipped\n"
fi

OS="$(uname -s)"

case $OS in
*"Linux"*)
  # Install QT5 for the User Interface
  printf "Installing qt5-default\n"
  sudo apt install -y qt5-default

  # For unit testing
  printf "Installing unit testing dependencies\n"
  sudo apt install libboost-dev libboost-all-dev -y
  ;;
*"Darwin"*)
  # Install QT5 for the User Interface
  printf "Installing qt5\n"
  brew install qt5

  # For unit testing
  printf "Installing unit testing dependencies\n"
  brew install boost
  ;;
*)
  printf "Failed to install dependencies\n"
  exit 1
  ;;
esac

## Reset directory to project root
cd $PP_ROOT

printf "\nSuccessfully installed dependencies\n"
