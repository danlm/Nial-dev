#Building the core version of QNial7 

This directory is used to build an executable version of QNial7 that supports the core capabilities.
It is used for platforms that are not in the set of supported platforms described in the QNial7 README.


The directory BuildCore has the following structure:


Entry          | Contents                                   
-------------- | ---------------------------------------- 
README.md      | The file you are currently reading 
build          | The directory used by CMake, initially empty
src            | The core source code and a CmakeLists.txt file


##Requirements

The build process uses CMake and requires that you have installed this. You
can use either the command line version of CMake or the GUI.

The GNU readline library is also required. This should be present by default
on OSX but may need to added to Linux distributions.

Both the CLANG compiler and GCC can be used to build Nial so that you will 
needto have installed one of these compilers. On OSX you should install
*XCode*, on Linux use the appropriate package manager for your distribution.
CLANG appears to generate slightly faster code on Linux.


##Building the core

Use the CMake tool to build  the "nialcore" executable with the following
steps:

1. Ensure that the "build" directory is empty.

2. Set up CMake to point at the Buildcore/src directory as the src
   directory and BuildCore/build as the build directory.

3. Change into build and execute "make".

   $ cd build
   $ make

   The result will be the executable "nialcore" in the build directory.

   Test it by running nialcore interactively with the command

   $ ./nialcore -i

   which will display a header like:

   Q'Nial V7.0 Open Source Edition Intel x86 64bit Mac OSX May 19 2017
   Copyright (c) NIAL Systems Limited
   clear workspace created

   and will be awaiting a nial input indented 6 spaces. Type:

        5 + tell 10

   which will display

   5 6 7 8 9 10 11 12 13 14

   Type:

       bye

   to end the nial interactive session.

4. Copy the nialcore executable to the BuildNial directory using

   $ cp nialcore ../BuildNial


You are now ready to build the full QNial7 using pkgblder in BuildNial.

