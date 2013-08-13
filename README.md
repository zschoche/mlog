# mlog

a comfortable lightweight C++ logging library -- header-only, cross-platform, C++11.

## Prolog

Building this library first came to my mind when I was working with Boost.Log and experienced some problems with it.

Don't get me wrong Boost.Log is a great library, but one day when I started building a sandboxed app for Apple's OSX 10.8, Boost.Log gave me a headache.

I thought a bit about logging in general and decided to create a lightweight logging library. My goal was to build a library which is as comfortable as possible and which has as few logging overhead as possible.

## What makes this library lightweight?

 - The preprocessor flag `MLOG_NO_LIB` make this library header-only.

 - Here are the speed test results on my MacBook Air (1,3 GHz Intel Core i5, 8GB RAM, SSD):
```
>	### single-threaded standard logger test ###
>	0.003ms for each log statment.
>	0.003ms for each log statment with thread id.
>	0.005ms for each log statment with thread id and timestamp.
>	0.0085ms for each log statment with thread id, timestamp and position.
>	
>	### multi-threaded standard logger test ###
>	0.017ms for each log statment.
>	0.026ms for each log statment with thread id.
>	0.0375ms for each log statment with thread id and timestamp.
>	0.043ms for each log statment with thread id, timestamp and position.
>	
>	### single-threaded memory logger test ###
>	0.000105ms for each log statment.
>	0.0001ms for each log statment with thread id.
>	0.000135ms for each log statment with thread id and timestamp.
>	0.000135ms for each log statment with thread id, timestamp and position.
>	
>	### multi-threaded memory logger test ###
>	0.000185ms for each log statment.
>	0.000175ms for each log statment with thread id.
>	0.000185ms for each log statment with thread id and timestamp.
>	0.000185ms for each log statment with thread id, timestamp and position.
>	
>	### single-threaded file logger test ###
>	0.0012ms for each log statment.
>	0.00159ms for each log statment with thread id.
>	0.002115ms for each log statment with thread id and timestamp.
>	0.00244ms for each log statment with thread id, timestamp and position.
>	
>	### multi-threaded file logger test ###
>	0.012135ms for each log statment.
>	0.015715ms for each log statment with thread id.
>	0.01851ms for each log statment with thread id and timestamp.
>	0.018715ms for each log statment with thread id, timestamp and position.
```
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

__Example:__ `MLOG_INFO("how to log");` and `MLOG_INFO(boost::format("how to log with format: %1%.") % 23);`

The next functions work with `std::stringstream`:

    MLOG_TRACE_STREAM()
    MLOG_DEBUG_STREAM()
    MLOG_INFO_STREAM()
    MLOG_WARNING_STREAM()
    MLOG_ERROR_STREAM()
    MLOG_FATAL_STREAM()

__Example:__ `MLOG_INFO_STREAM("how to log " << "with a stream");`

The trace and debug log statments only work if __MLOGDEBUG__ and __MLOGTRACE__ are defined as a preprocessor flag. If this is not the case the debug and trace statments will be ignored completely. These statements won't affect the performance of your program any more.
I just recommand you to use [Boost.Format](http://www.boost.org/doc/libs/1_52_0/libs/format/) is stead of the std::stringstream version. It's faster.

## Building with CMake

To build the library and with CMake, you will need to
have CMake version 2.6 or higher installed appropriately in your
system.

    $ cmake --version
    cmake version 2.8.1

Inside the mlog project directory, you can issue the following statements to
configure and generate the Makefiles.

    $ cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

Once CMake is done with generating the Makefiles and configuring the project,
you can now build the library.

    $ make

## Log Destination

The `mlog::mlogger` is a static `std::unique_ptr<mlog::logger>` and contains the current log destination.
The standard destination is a `mlog::standard_logger`. Reset the `mlog::mlogger` to use another destination.

Example:
```c++
#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>
    
//...
    
mlog::mlogger.reset(new mlog::file_logger("log.txt"));
MLOG_TRACE("Write this into log.txt");
```

This library is already shipped with the followed logger types:

<table>
  <tr>
    <th>Logger Name</th>
    <th>Description</th>
  </tr>
  <tr>
    <td>mlog::standard_logger</td>
    <td>Is logging into std::cout</td>
  </tr>
  <tr>
    <td>mlog::standard_logger_thread_safe</td>
    <td>Is logging thread safe into std::cout</td>
  </tr>
  <tr>
    <td>mlog::file_logger</td>
    <td>Is logging into a file</td>
  </tr> 
  <tr>
    <td>mlog::file_logger_thread_safe</td>
    <td>Is logging thread safe into a file</td>
  </tr>
  <tr>
    <td>mlog::memory_logger_normal</td>
    <td>Is logging thread safe into the memory<br>(holds the last 4096 log entrys)</td>
  </tr>
  <tr>
    <td>mlog::memory_logger_big</td>
    <td>Is logging thread safe into the memory<br>(holds the last 65535 log entrys)</td>
  </tr>
</table>

### Custom Log Infrastructure

It is possible to create your own logger class if you want to write your log at some place in the network or something like that. You just have to write a logger class and overload the `mlog::logger::write_to_log()` function.

__mlog::logger interface:__
```c++
namespace mlog
{

	class logger
	{
	public:

		virtual void write_to_log(log_metadata&& metadata, std::string&& log_text) = 0;
	};
}

```
