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
  git clone https://github.com/tensorflow/tensorflow.git tensorflow_src
fi
cd tensorflow_src
git fetch
git checkout $TF_VERSION
cd ..

## Build TFL
# echo "Installing TensorFlow Lite ..."
# mkdir -p tflite_build
# cd tflite_build
# cmake ../tensorflow_src/tensorflow/lite
# cmake --build .

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
