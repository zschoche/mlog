/*
 * mlog.hpp
 *
 *  Created on: Aug 12, 2012
 *      Author: philipp
 */

#ifndef MLOG_HPP_
#define MLOG_HPP_

#include "logger.hpp"
#include "leveled_logger.hpp"
#include <boost/shared_ptr.hpp>

namespace mlog
{

extern boost::shared_ptr<logger> mlogger;

template<class logger_class>
void init();

class log : public leveled_logger
{
public:
	log(mlog_level level)
	:leveled_logger(level, mlogger.get())
	{

	}

	virtual ~log()
	{
	}
};

class log_trace : public log
{
public:
	log_trace()
	:log(trace)
	{ }
	
	virtual ~log_trace()
	{
		get_logger()->flush();
	}
};

class log_debug : public log
{
public:
	log_debug()
	:log(debug)
	{ }

	virtual ~log_debug()
	{
		get_logger()->flush();
	}

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
	
	virtual ~log_error()
	{
		get_logger()->flush();
	}

};

class log_fatal : public log
{
public:
	log_fatal()
	:log(fatal)
	{ }

	virtual ~log_fatal()
	{
		get_logger()->flush();	
	}
};

}; /* namespace mlog */


#ifdef MLOGTRACE
#define MLOG_TRACE(x1) mlog::log_trace() << x1
#else
#define MLOG_TRACE(x1)
#endif


#ifdef MLOGDEBUG
#define MLOG_DEBUG(x1) mlog::log_debug() << x1
#else
#define MLOG_DEBUG(x1)
#endif

#define MLOG_INFO(x1) mlog::log_info() << x1
#define MLOG_WARNING(x1) mlog::log_warning() << x1
#define MLOG_ERROR(x1) mlog::log_error() << x1
#define MLOG_FATAL(x1) mlog::log_fatal() << x1

#endif /* MLOG_HPP_ */
