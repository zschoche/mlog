# mlog


C++ logging library to disable log statements via preprocessor.



## Getting started

### Building with CMake

To build the library and with CMake, you will need to
have CMake version 2.8 or higher installed appropriately in your
system.

    $ cmake --version
    cmake version 2.8.1

Inside the mlog project directory, you can issue the following statements to
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
    	MLOG_DEBUG("write " << 1 << " debug line to your log."); // works only with MLOGDEBUG
    	MLOG_INFO("write a info line to your log.");
    	MLOG_ERROR("write a error line to your log.");
    	MLOG_FATAL("write a fatal line to your log.");
    	return 0;
    }

The default log target is std::cout.
If you want to log into a file then use the file_logger or file_logger_thread_safe.


    #include <mlog/mlog.hpp>
    #include <mlog/file_logger.hpp>

    int main()
    {
    	mlog::mlogger.reset(new mlog::file_logger("log.txt", ".", 1024 * 1024 * 5)); //5MB log files in current directory 

    	MLOG_TRACE("write a trace line to your log."); // works only with MLOGTRACE
    	MLOG_DEBUG("write " << 1 << " debug line to your log."); // works only with MLOGDEBUG
    	MLOG_INFO("write a info line to your log.");
    	MLOG_ERROR("write a error line to your log.");
    	MLOG_FATAL("write a fatal line to your log.");
    	return 0;
    }

If you want to write your logs to a completly different target then you have to implement your own logger and reset mlog::mlogger.
Your own logger must inherit from mlog::logger and overwrite the functions write_to_log() and flush().

    namespace mlog
    {
    
    class logger
    {
    public:
    	virtual void flush();
    	virtual void write_to_log(const std::string& log_text);
    };
    
    }

