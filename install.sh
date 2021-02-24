#! /bin/sh

while getopts ":t" opt; do
  case ${opt} in
    t) TESTING=1
  esac
done

## Install dependecies
if [ $TESTING -eq 1 ]
then
  ./install_dependencies.sh -t
else
  ./install_dependencies.sh
fi



## Build PosturePerfection
mkdir -p build
cmake . -B build
cmake --build build

cd ..

echo ""
echo ""
echo "PosturePerfection has been installed!"
echo "To start the application run:"
echo "    ./build/PosturePerfection"
