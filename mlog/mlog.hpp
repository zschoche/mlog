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
	log(mlog_level&& level, log_position position)
	:leveled_logger(std::move(level), mlogger.get(),std::move(position))
	{ }
};

class log_trace : public log
{
public:
	log_trace(log_position position)
	:log(trace, std::move(position))
	{ }
};

class log_debug : public log
{
public:
	log_debug(log_position position)
	:log(debug, std::move(position))
	{ }
};

class log_info : public log
{
public:
	log_info(log_position position)
	:log(info, std::move(position))
	{ }
};

class log_warning : public log
{
public:
	log_warning(log_position position)
	:log(warning, std::move(position))
	{ }
};

class log_error : public log
{
public:
	log_error(log_position position)
	:log(error, std::move(position))
	{ }
};

class log_fatal : public log
{
public:
	log_fatal(log_position position)
	:log(fatal, std::move(position))
	{ }
};


}; /* namespace mlog */


#ifdef MLOGTRACE
#define MLOG_TRACE_STREAM(x1) ::mlog::log_trace(log_position(__FILE__, __LINE__)) << x1
#define MLOG_TRACE(x1) ::mlog::mlogger->write(mlog_level::trace, x1, ::mlog::log_position(__FILE__, __LINE__))

#else
#define MLOG_TRACE_STREAM(x1)
#define MLOG_TRACE(x1)
#endif


#ifdef MLOGDEBUG
#define MLOG_DEBUG_STREAM(x1) ::mlog::log_debug(::mlog::log_position(__FILE__, __LINE__)) << x1
#define MLOG_DEBUG(x1) ::mlog::mlogger->write(mlog_level::debug, x1, ::mlog::log_position(__FILE__, __LINE__)) 
#else
#define MLOG_DEBUG_STREAM(x1)
#define MLOG_DEBUG(x1)
#endif

#define MLOG_INFO_STREAM(x1) ::mlog::log_info(::mlog::log_position(__FILE__, __LINE__)) << x1
#define MLOG_INFO(x1) ::mlog::mlogger->write(mlog_level::info, x1, ::mlog::log_position(__FILE__, __LINE__))

#define MLOG_WARNING_STREAM(x1) mlog::log_warning(::mlog::log_position(__FILE__, __LINE__)) << x1
#define MLOG_WARNING(x1) ::mlog::mlogger->write(mlog_level::warning, x1, ::mlog::log_position(__FILE__, __LINE__)) 

#define MLOG_ERROR_STREAM(x1) mlog::log_error(::mlog::log_position(__FILE__, __LINE__)) << x1
#define MLOG_ERROR(x1) ::mlog::mlogger->write(mlog_level::error, x1, ::mlog::log_position(__FILE__, __LINE__)) 

#define MLOG_FATAL_STREAM(x1) mlog::log_fatal(::mlog::log_position(__FILE__, __LINE__)) << x1
#define MLOG_FATAL(x1) ::mlog::mlogger->write(mlog_level::fatal, x1, ::mlog::log_position(__FILE__, __LINE__)) 

#endif /* MLOG_HPP_ */

#ifdef MLOG_NO_LIB
#include "impl/mlog.hpp"
#endif
