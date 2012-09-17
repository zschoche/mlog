mlog
====

C++ logging library to disable log statements via preprocessor.



Getting started
===============

Building with CMake
~~~~~~~~~~~~~~~~~~~


To build the libraries and run the tests with CMake, you will need to
have CMake version 2.8 or higher installed appropriately in your
system.

::

    $ cmake --version
    cmake version 2.8.1

Inside the mlog directory, you can issue the following statements to
configure and generate the Makefiles.

$ cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

Once CMake is done with generating the Makefiles and configuring the project,
you can now build the library::

    $ make


