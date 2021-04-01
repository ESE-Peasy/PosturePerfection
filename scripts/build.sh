#! /bin/sh

GREEN='\033[0;32m'
PURPLE='\033[0;35m'
RED='\033[0;31m'
NO_COLOUR='\033[0m'

exit_on_build_error() {
  printf "${RED}Build error(s)${NO_COLOUR}: check compile_log.txt for details\n"
  exit 1
}

printf "Starting build...\n"

while getopts ":enable-testing" opt; do
  case ${opt} in
  t) ENABLE_TESTING=1 ;;
  esac
done

## Build PosturePerfection
mkdir -p build

if [ $ENABLE_TESTING ]; then
  printf "${PURPLE}Testing is enabled. Running cpplint...${NO_COLOUR}\n"
  cpplint --filter=-build/include_subdir --recursive --quiet ./src/* || exit 1
  printf "${GREEN}DONE - Formatting looks good!${NO_COLOUR}\n"

  printf "Building the project...\n"
  cmake . -B build -DENABLE_TESTING=True \
    2>compile_log.txt || exit_on_build_error

  cmake --build build \
    2>compile_log.txt || exit_on_build_error

  printf "${PURPLE}Running ctest...${NO_COLOUR}\n"
  cd build
  ctest --verbose || {
    printf "\n${RED}One or more tests failed. Please see the error messages above " \
      "for more information.${NO_COLOUR}\n"
    exit 1
  }
  printf "\n${GREEN}All tests have passed. Ready to push to GitHub!\n"
  cd ..
else
  cmake . -B build -DENABLE_TESTING=False \
    2>compile_log.txt || exit_on_build_error

  cmake --build build \
    2>compile_log.txt || exit_on_build_error

  printf "\n${GREEN}PosturePerfection has been built!${NO_COLOUR}\n"
  printf "Any compiler logs have been written to compile_log.txt\n"
fi

cp "./build/src/PosturePerfection" .

printf "\n${GREEN}To start the application run:\n"
printf "    ./PosturePerfection${NO_COLOUR}\n"
