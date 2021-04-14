#! /bin/sh

# Note: we install TensorFlow and OpenCV via zip files hosted on GitHub. We do this
# because the git clone operation takes too long and sometimes fails on lower-end
# systems.

printf "Installing dependencies for PosturePerfection\n"

PP_ROOT=$(pwd)
MIN_CMAKE="3.16"
TF_VERSION="f32b4d37cc059771198484d453d5cf288cf7803b"
CV_VERSION="69357b1e88680658a07cffde7678a4d697469f03"

# Makes use of snippet from: https://stackoverflow.com/a/3278427 for
# cleanly checking git commits of dependencies
UPSTREAM=${1:-'@{u}'}

## Check for correct version of CMake
if ! cmake --version >/dev/null 2>&1; then
  printf "CMake is not installed, please install version 3.16 or higher\n"
  printf "Perhaps run:\n"
  printf "    pip3 install cmake\n"
  printf "You may also need to add the pip install directory to your PATH:\n"
  printf "    PATH=\$PATH:/home/pi/.local/bin\n"
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
  cd cppTimer_src
  LOCAL=$(git rev-parse @)
  REMOTE=$(git rev-parse "$UPSTREAM")
  if [ $LOCAL = $REMOTE ]; then
    printf " skipped\n"
  else
    printf " downloading update\n"
    make clean
    git pull origin HEAD
    cmake .
    make
    sudo make install
  fi
  cd ..
  LOCAL=""
  REMOTE=""
fi

## Download cppTimer
printf "Downloading simple-remote-notify-send ..."
if [ ! -d "simple-remote-notify-send_src" ]; then
  printf "\n"
  git clone https://github.com/ESE-Peasy/simple-remote-notify-send simple-remote-notify-send_src || exit 1
  cd simple-remote-notify-send_src
  cmake .
  make
  sudo make install
  cd ..
else
  cd simple-remote-notify-send_src
  LOCAL=$(git rev-parse @)
  REMOTE=$(git rev-parse "$UPSTREAM")
  if [ $LOCAL = $REMOTE ]; then
    printf " skipped\n"
  else
    printf " downloading update\n"
    make clean
    git pull origin HEAD
    cmake .
    make
    sudo make install
  fi
  cd ..
  LOCAL=""
  REMOTE=""
fi

## Download TFL code
printf "Downloading TensorFlow ..."
if [ ! -d "tensorflow_src" ]; then
  printf "\n"
  wget "https://github.com/tensorflow/tensorflow/archive/$TF_VERSION.zip" || exit 1
  unzip -q $TF_VERSION
  mv "tensorflow-$TF_VERSION" tensorflow_src
else
  printf " skipped\n"
fi

## Download OpenCV code
printf "Downloading OpenCV ..."
if [ ! -d "opencv_src" ]; then
  printf "\n"
  wget "https://github.com/opencv/opencv/archive/$CV_VERSION.zip" || exit 1
  unzip -q $CV_VERSION
  mv "opencv-$CV_VERSION" opencv_src
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

# Install cpplint for testing purposes
pip3 install cpplint || printf "Installing cpplint failed\n"

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
