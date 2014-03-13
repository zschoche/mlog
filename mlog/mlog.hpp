/*
 * 
 * mlog.hpp
 *
 *  Created on: Aug 12, 2012
 *      Author: philipp
 */

#ifndef __MLOG_HPP_
#define __MLOG_HPP_

#include "logger.hpp"
#include "thread_safe.hpp"
#include "standard_logger.hpp"
#include "manager.hpp"
#include <atomic>
#include <memory>

namespace mlog {

// This should not be cleaned up https://github.com/zschoche/mlog/issues/11
extern mlog_manager *manager;

unsigned int pseudo_random_number(unsigned int max = 99);

}; /* namespace mlog */

#ifdef MLOGTRACE
#define MLOG_TRACE(x1)                                                         \
	::mlog::manager->log()->write(                                         \
				    mlog_level::trace, x1,                     \
				    ::mlog::log_position(__FILE__, __LINE__))

#else
#define MLOG_TRACE(x1)
#endif

#ifdef MLOGDEBUG
#define MLOG_DEBUG(x1)                                                         \
	::mlog::manager->log()->write(                                         \
				    mlog_level::debug, x1,                     \
				    ::mlog::log_position(__FILE__, __LINE__))
#else
#define MLOG_DEBUG(x1)
#endif

#define MLOG_INFO(x1) if(::mlog::manager->use_position()) { ::mlog::manager->log()->write(mlog_level::info, x1, ::mlog::log_position(__FILE__, __LINE__)); } else { ::mlog::manager->log()->write(mlog_level::info, x1); }

#define MLOG_WARNING(x1)                                                       \
	::mlog::manager->log()->write(                                         \
				    mlog_level::warning, x1,                   \
				    ::mlog::log_position(__FILE__, __LINE__))

#define MLOG_ERROR(x1)                                                         \
	::mlog::manager->log()->write(                                         \
				    mlog_level::error, x1,                     \
				    ::mlog::log_position(__FILE__, __LINE__))

#define MLOG_FATAL(x1)                                                         \
	::mlog::manager->log()->write(                                         \
				    mlog_level::fatal, x1,                     \
				    ::mlog::log_position(__FILE__, __LINE__))

#endif /* MLOG_HPP_ */

#ifdef MLOG_NO_LIB
#include "impl/mlog.hpp"
#endif
