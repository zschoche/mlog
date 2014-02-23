# mlog

Comfortable lightweight C++ logging library -- header-only, cross-platform, C++11.

## Prolog

Building this library first came to my mind when I was working with Boost.Log and experienced some problems with it.

Don't get me wrong Boost.Log is a great library, but one day when I started building a sandboxed app for Apple's OSX 10.8, Boost.Log gave me a headache.

I thought a bit about logging in general and decided to create a lightweight logging library. My goal was to build a library which is as comfortable as possible and which has as few logging overhead as possible.

## What makes this library lightweight?

 - The preprocessor flag `MLOG_NO_LIB` make this library header-only.

 - Here are the speed test results on my MacBook Air (1,3 GHz Intel Core i5, 8GB RAM, SSD):
```
### single-threaded standard logger test ###
0.003ms for each log statment.
0.003ms for each log statment with thread id.
0.005ms for each log statment with thread id and timestamp.
0.015ms for each log statment with thread id, timestamp and position.

### multi-threaded standard logger test ###
0.003ms for each log statment.
0.005ms for each log statment with thread id.
0.005ms for each log statment with thread id and timestamp.
0.015ms for each log statment with thread id, timestamp and position.

### memory logger test ###
6.5e-05ms for each log statment.
6.5e-05ms for each log statment with thread id.
9e-05ms for each log statment with thread id and timestamp.
8.5e-05ms for each log statment with thread id, timestamp and position.

### single-threaded file logger test ###
0.00115ms for each log statment.
0.00164ms for each log statment with thread id.
0.002055ms for each log statment with thread id and timestamp.
0.00229ms for each log statment with thread id, timestamp and position.

### multi-threaded file logger test ###
0.00117ms for each log statment.
0.00153ms for each log statment with thread id.
0.00218ms for each log statment with thread id and timestamp.
0.00298ms for each log statment with thread id, timestamp and position.
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

The trace and debug log statments only work if __MLOGDEBUG__ and __MLOGTRACE__ are defined as a preprocessor flag. If this is not the case the debug and trace statments will be ignored completely. These statements won't affect the performance of your program any more.

## Dependences

 * [Boost System Library](www.boost.org/libs/system/)
 * [Boost Format Library](www.boost.org/libs/format/)
 * [Boost.Random](www.boost.org/libs/random/)
 

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

The `mlog::manager` is pointing to the the current log destination.
The standard destination is a `mlog::standard_logger`. use `mlog::manager->set_log(...)` to set your own log destination.

Example:
```c++
#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>
    
//...
    
mlog::file_logger logfile("log.txt");
mlog::manager->set_log(logfile);
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
    <td>Is logging thread safe into the memory<br>(holds the last 4096 log entries)</td>
  </tr>
  <tr>
    <td>mlog::memory_logger_big</td>
    <td>Is logging thread safe into the memory<br>(holds the last 65535 log entries)</td>
  </tr>
  <tr>
    <td>mlog::syslog_logger</td>
    <td>It's using the syslog() <br>(unix only)</td>
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
		virtual void write_to_log(const log_metadata& metadata, const std::string& log_text) = 0;
	};
}

```
