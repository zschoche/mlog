/*
 * mlog.hpp
 *
 *  Created on: Aug 12, 2012
 *      Author: philipp
 */

#ifndef __MLOG_HPP_
#define __MLOG_HPP_

#include "logger.hpp"
#include "leveled_logger.hpp"
#include "thread_safe.hpp"
#include "standard_logger.hpp"
#include <memory>

namespace mlog
{

extern std::unique_ptr<logger> mlogger;

template<class logger_class>
void init();

class log : public leveled_logger
{
public:
	log(mlog_level&& level)
	:leveled_logger(std::move(level), mlogger.get())
	{ }
};

class log_trace : public log
{
public:
	log_trace()
	:log(trace)
	{ }
};

class log_debug : public log
{
public:
	log_debug()
	:log(debug)
	{ }
};

class log_info : public log
{
public:
	log_info()
	:log(info)
	{ }
};

class log_warning : public log
{
public:
	log_warning()
	:log(warning)
	{ }
};

class log_error : public log
{
public:
	log_error()
	:log(error)
	{ }
};

class log_fatal : public log
{
public:
	log_fatal()
	:log(fatal)
	{ }
};


}; /* namespace mlog */


#ifdef MLOGTRACE
#define MLOG_TRACE_STREAM(x1) ::mlog::log_trace() << x1
#define MLOG_TRACE(x1) ::mlog::mlogger->write(mlog_level::trace, x1)

#else
#define MLOG_TRACE_STREAM(x1)
#define MLOG_TRACE(x1)
#endif


#ifdef MLOGDEBUG
#define MLOG_DEBUG_STREAM(x1) ::mlog::log_debug() << x1
#define MLOG_DEBUG(x1) ::mlog::mlogger->write(mlog_level::debug, x1) 
#else
#define MLOG_DEBUG_STREAM(x1)
#define MLOG_DEBUG(x1)
#endif

#define MLOG_INFO_STREAM(x1) ::mlog::log_info() << x1
#define MLOG_INFO(x1) ::mlog::mlogger->write(mlog_level::info, x1)

#define MLOG_WARNING_STREAM(x1) mlog::log_warning() << x1
#define MLOG_WARNING(x1) ::mlog::mlogger->write(mlog_level::warning, x1) 

#define MLOG_ERROR_STREAM(x1) mlog::log_error() << x1
#define MLOG_ERROR(x1) ::mlog::mlogger->write(mlog_level::error, x1) 

#define MLOG_FATAL_STREAM(x1) mlog::log_fatal() << x1
#define MLOG_FATAL(x1) ::mlog::mlogger->write(mlog_level::fatal, x1) 

#endif /* MLOG_HPP_ */

#ifdef MLOG_NO_LIB
#include "impl/mlog.hpp"
#endif
