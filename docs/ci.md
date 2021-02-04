---
nav_order: 5
title: Continuous Integration
---

# Continuous Integration
GitHub *Actions* are being used to perform CI on this project. Below is a list of all jobs and any notes.
{: .fs-6 .fw-300 }

## Doxygen Documentation Generation
Automatic generation of source code documentation from comments, performed by Doxygen. In this project the headers are to be documented, so source code documentation can be found in the interface specified by the header files. To ensure the file is included in the documentation it needs a `@file` comment at the top. Only comments relating to implementation details should be in the C source files. All usage documentation should be in the header files.

A minimal header file may therefore be:
```cpp
/**
 * @file hello.h
 * @brief A hello world function
 * 
 */

#ifndef SRC_HELLO_H_
#define SRC_HELLO_H_

/**
 * @brief Example to print "Hello, world!"
 * 
 */
void hello_world(void);
#endif  // SRC_HELLO_H_
```
**Note** the `@file` at the top if the file to ensure this is included in the documentation.

## CMake
This job attempts to build the project and then runs tests as defined in the CMake configuration. See See https://cmake.org/cmake/help/latest/manual/ctest.1.html for details. If this passes, the project can be built and all unit tests will have passed.

## C++ Linting
This job runs the `cpplint` python package on all code. This checks against Google code formatting guidelines, with some minor modifications. See the [project page](https://github.com/cpplint/cpplint) for details.

The following checks are disabled at the moment:
- `legal/copyright`
  - Would require a copyright statement at the top of every file
- `build/include_subdir`
  - Would require subdirectory paths to be specified when including headers
  - We may want to enable this again if we change the directory structure of the project

To ensure adherence to the desired coding style before pushing you can install `cpplint` locally using `pip install cpplint`. Check the documentation or run `cpplint --help` for usage instructions. An example run might be `cpplint --filter=-legal/copyright src/*`.

You can also set the `C_Cpp.clang_format_style` setting (from the C/C++ extension) in VSCode to "Google", to use Google style formatting. Remember you can apply formatting at any time using the keyboard shortcut `Ctrl`+`Shift`+`i` in VSCode.
