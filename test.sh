#! /bin/bash

echo "Starting ..."

## Check for cpplint
if ! cpplint --version >/dev/null 2>&1
then
    echo "cpplint is not installed; please install with:"
    echo "    pip3 install cpplint"
    exit 1
fi

printf "Running cpplint ... \r"
cpplint --filter=-build/include_subdir --recursive --quiet ./src/*
echo "Running cpplint ... DONE"

echo "Starting build ... "

./install.sh -t || { echo "Problem building, see above for specific errors"; exit 1; }

echo "Running ctest ... "

cd build
ctest || { cd ..; exit 1; }

cd ..
