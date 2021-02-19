#! /bin/sh

## Install dependecies
./install_dependencies.sh

## Build PosturePerfection
mkdir -p build
cd build
cmake ../src
cmake --build .

cd ..

echo ""
echo ""
echo "PosturePerfection has been installed!"
echo "To start the application run:"
echo "    ./build/PosturePerfection"
