#! /bin/sh

echo "Starting installation ... "

while getopts ":t" opt; do
  case ${opt} in
    t) TESTING=1
  esac
done

## Install dependecies
if [ $TESTING ]
then
  ./install_dependencies.sh -t
else
  ./install_dependencies.sh
fi



## Build PosturePerfection
mkdir -p build

cmake . -B build || exit 1

cmake --build build || exit 1

cd ..

echo ""
echo ""
echo "PosturePerfection has been installed!"
echo "To start the application run:"
echo "    ./build/PosturePerfection"
