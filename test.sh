#! /bin/bash

echo "Starting ..."

if ! cpplint --version >/dev/null 2>&1
then
    echo "cpplint is not installed; please install with:"
    echo "    pip3 install cpplint"
    exit 1
fi

printf "Running cpplint ... \r"
cpplint --filter=-build/include_subdir --recursive --quiet ./src/*
echo "Running cpplint ... DONE"

echo "Building the project ... "

mkdir -p build
cmake . -B build || exit 1
cmake --build build || exit 1

echo "Running ctest ... "

cd build
ctest -VV  || { cd ..; exit 1; }

cd ..
