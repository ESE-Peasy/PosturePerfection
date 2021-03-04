#! /bin/sh

GREEN='\033[0;32m'
PURPLE='\033[0;35m'
RED='\033[0;31m'
NO_COLOUR='\033[0m'

echo "Starting build..."

while getopts ":enable-testing" opt; do
  case ${opt} in
    t) ENABLE_TESTING=1
  esac
done

## Build PosturePerfection
mkdir -p build

if [ $ENABLE_TESTING ]; then
  echo "${PURPLE}Testing is enabled. Running cpplint..."
  cpplint --filter=-build/include_subdir --recursive --quiet ./src/* || exit 1
  echo "${GREEN}DONE - Formatting looks good!${NO_COLOUR}\n"
  echo "Building the project ... "
  cmake . -B build -DENABLE_TESTING=True || exit 1
  cmake --build build || exit 1
  echo "${PURPLE}Running ctest...${NO_COLOUR}"
  cd build
  ctest --verbose || { echo "\n${RED}One or more tests failed. Please see the error messages above for more information.\n" ; exit 1; }
  echo "\n${GREEN}All tests have passed. Ready to push to GitHub!"
  cd ..
else
  cmake . -B build -DENABLE_TESTING=False || exit 1
  cmake --build build || exit 1
  echo "\n${GREEN}PosturePerfection has been built!"
fi

cp "./build/src/PosturePerfection" .

echo "\nTo start the application run:"
echo "    ./PosturePerfection"
