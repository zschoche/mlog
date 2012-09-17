# mlog


C++ logging library to disable log statements via preprocessor.



## Getting started

### Building with CMake

To build the libraries and run the tests with CMake, you will need to
have CMake version 2.8 or higher installed appropriately in your
system.

    $ cmake --version
    cmake version 2.8.1

Inside the mlog directory, you can issue the following statements to
configure and generate the Makefiles.

    $ cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

Once CMake is done with generating the Makefiles and configuring the project,
you can now build the library.

    $ make

### How to create logs 

Add this mlog project directory as an include directory and the libmlog.a into your project.

Now you can enable/disable trace and debug logging with preprocessor flags. MLOGTRACE for trace and MLOGDEBUG for debug.


    #include <mlog/mlog.hpp>

    int main()
    {
    	MLOG_TRACE("write a trace line to your log."); // works only with MLOGTRACE
    	MLOG_DEBUG("write a debug line to your log."); // works only with MLOGDEBUG
    	MLOG_INFO("write a info line to your log.");
    	MLOG_ERROR("write a error line to your log.");
    	MLOG_FATAL("write a fatal line to your log.");
    	return 0;
    }

