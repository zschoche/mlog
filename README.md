# mlog [![Build Status](https://travis-ci.org/zschoche/mlog.png?branch=master)](https://travis-ci.org/zschoche/mlog) [![Build status](https://ci.appveyor.com/api/projects/status/vg10iijgbu67htwp?svg=true)](https://ci.appveyor.com/project/zschoche/mlog)

Comfortable lightweight C++ logging library -- cross-platform, C++11. 

## Prolog

Boost.Log is a great library (especially v2.0 which was not ready at the beginning of mlog), but one day when I started building a sandboxed app for Apple's OSX 10.8, Boost.Log gave me a headache.

I decided to start a lightweight logging library. The goal is to have a library which is as comfortable as possible and with an as low as possible logging performance overhead.

## What makes this library lightweight?

During development or beta phases, you'll often want to make heavy use of log statements but you'll want to build a release-version of your app and remove all log statements of specific log levels used in development, which means that there is zero performance overhead for those statements. You can do this easily with a different set of preprocessor flags to disable certain log-levels. No instructions of the disabled log statements will even be compiled into the binary.  

## What makes this library comfortable?

Basically you just have to include the __mlog/mlog.hpp__ and use one of the logging macro functions.
```c++
#include <mlog/mlog.hpp>

//...

MLOG_INFO("this is your first log entry.")
```
There are 12 different macro functions.

The follow macro functions accept `char*`, `std::string` and `boost::format`:

    MLOG_TRACE()
    MLOG_DEBUG()
    MLOG_INFO()
    MLOG_WARNING()
    MLOG_ERROR()
    MLOG_FATAL()

__Example:__ `MLOG_INFO("how to log");` and `MLOG_INFO(boost::format("how to log with format: %1%.") % 42);`

The trace and debug log statments only work if __MLOGDEBUG__ and __MLOGTRACE__ are defined as a preprocessor flag. If this is not the case the debug and trace statments will be ignored completely, and these statements won't affect the performance of your program.



## Benchmark
Here are the benchmark results on my MacBook Air (1,3 GHz Intel Core i5, 8GB RAM, SSD):
```
### mlog benchmark ###

compare with boost.log on stdout :
	boost.log:	0.004365ms
	mlog:		0.004465ms
	
compare with boost.log on files:
	boost.log:	0.000945ms
	mlog:		0.000775ms
	
frontend tests:
	default settings =>	0.00018ms
	added thread id =>	0.000415ms
	added time =>		0.00074ms
	added position time =>	0.00094ms
	
cout tests:
	mlog::standard_logger => 0.002205ms
	mlog::thread_safe<mlog::standard_logger> => 0.00225ms
	mlog::async_logger<mlog::standard_logger> => 0.000125ms
		with thread id:		+0.00079ms
		with time:		+0.001215ms
		with code position:	+0.001585ms
		
file tests:
	mlog::file_logger => 0.00104ms
	mlog::thread_safe<mlog::file_logger> => 0.00103ms
	mlog::async_logger<mlog::file_logger> => 0.00012ms
		with thread id:		-0.000175ms
		with time:		-0.00051ms
		with code position:	-0.00065ms
		
syslog tests:
	mlog::syslog_logger => 0.01322ms
	mlog::thread_safe<mlog::syslog_logger> => 0.002255ms
	mlog::async_logger<mlog::syslog_logger> => 0.000165ms
		with thread id:		-0.01074ms
		with time:		-0.01064ms
		with code position:	-0.010675ms
		
memory tests:
	mlog::memory_logger => 2e-05ms
		with thread id:		0ms
		with time:		+2.5e-05ms
		with code position:	0ms

```

## Dependences

 * all
 	- [Boost Format Library](www.boost.org/libs/format/)
 * all pre C++11
 	- [Boost.Thread](www.boost.org/libs/thread/)
	- [Boost System Library](www.boost.org/libs/system/)
 * mlog::file_logger
	- [Boost System Library](www.boost.org/libs/system/)
	- [Boost Filesystem Library](www.boost.org/libs/filesystem/)
 * test suite
 	- [Boost.Regex](www.boost.org/libs/regex/)
	- [Boost Test Library](www.boost.org/libs/test/)
	- [Boost.Log](http://www.boost.org/libs/log)
 

## Building with CMake

To build the library and with CMake, you will need to
have CMake version 2.6 or higher installed in your
system.

    $ cmake --version
    cmake version 2.8.1

Inside the mlog project directory, you can issue the following statements to
configure and generate the Makefiles.

    $ cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

Once CMake is done with generating the Makefiles and configuring the project,
you can build the library.

    $ make

## Log Destination

The `mlog::manager` is pointing to the the current log destination.
The standard destination is a `mlog::standard_logger`. Use `mlog::manager->set_log(...)` to set log destination.

Example:
```c++
#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>
    
//...
    
mlog::file_logger logfile("log.txt");
mlog::manager->set_log(logfile);
MLOG_TRACE("Write this into log.txt");
```

This library is shipped with the followed logger types.

### mlog::standard_logger
This logger is writing to the 'stdout'. There is also a thread-safe version of it: `mlog::standard_logger_thread_safe`.

### mlog::file_logger
This logger is writing to a file. There is also a thread-safe version of it: `mlog::file_logger_thread_safe`.

### mlog::memory_logger<X>
This logger is writing into the memory and keeps the last X log statments, which is thread-safe and the fastest one in this library. Go with the memory logger if your code is on a hot code path (or otherwise performance critical).
There are some pre-defined memory loggers:
* mlog::memory_logger_normal (holds the last 4096 log entries)
* mlog::memory_logger_big (holds the last 65535 log entries)

The Idea is to print the log only in a case of an error.

__Example:__
```c++
#include <mlog/mlog.hpp>
#include <mlog/memory_logger.hpp>
    
//...
    
mlog::memory_logger_normal log;
mlog::manager->set_log(log);
try {
	MLOG_TRACE("Write this into the memory.");
	//...
} catch(...) {
	log.output(std::cout); // something goes wrong. Lets have a look into the log.
}
```

### mlog::syslog_logger
This logger is only available on unix systems and uses the `syslog()` function.

### mlog::thread_safe<L>
`L` must be a `mlog::logger`. This makes `L` thread-safe.<br>

__Example:__
```c++
#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>
#include <mlog/thread_safe.hpp>
    
//...
    
mlog::thread_safe<mlog::file_logger> logfile("log.txt");
mlog::manager->set_log(logfile);
MLOG_TRACE("Write this into log.txt");
```

### mlog::async_logger<L>
`L` must be a `mlog::logger`. This logger is writing asynchron into `L`.<br>
__Note:__ That makes no sense for some loggers.(e.g. `mlog::memorylogger<>`)
__Example:__
```c++
#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>
#include <mlog/async_logger.hpp>
    
//...
    
mlog::async_logger<mlog::file_logger> logfile("log.txt");
mlog::manager->set_log(logfile);
MLOG_TRACE("Write this into log.txt");
logfile.flush();
```

### Custom Log Infrastructure

It is possible to create your own logger class if you want custom behaviour, like writing a logging file to a network-share. You just have to write a custom logger class and inherit from the `logger` template class by using the [CRTP](https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern)

__Example:__
```c++
struct custom_logger : mlog::logger<custom_logger>
{
	template<typename M, typename T>
	void write_to_log(M&& metadata, T&& log_text) {
		// write into your log:
		// std::string str = metadata.to_string(std::forward<T>(log_text), false);
		// ...
	}
};

//...

int main() {
	custom_logger log;
	mlog::manager->set_log(log);
	
	MLOG_INFO("It's done.");
	
	return 0;
}
```

## The Manager
coming soon...

## Filtering
coming soon...
