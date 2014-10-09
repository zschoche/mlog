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

template <typename T> void write_message(mlog_level &&lvl, T &&msg) {
	if (::mlog::manager->use_position()) {
		::mlog::manager->log()->write(
		    std::move(lvl), std::forward<T>(msg),
		    ::mlog::log_position(__FILE__, __LINE__));
	} else {
		::mlog::manager->log()->write(std::move(lvl),
					      std::forward<T>(msg));
	}
}

}; /* namespace mlog */

#ifdef MLOGTRACE
#define MLOG_TRACE(x1) mlog::write_message(mlog_level::trace, x1)
#else
#define MLOG_TRACE(x1)
#endif

#ifdef MLOGDEBUG
#define MLOG_DEBUG(x1) mlog::write_message(mlog_level::debug, x1)
#else
#define MLOG_DEBUG(x1)
#endif

#ifdef DISABLE_MLOG_INFO
#define MLOG_INFO(x1)
#else
#define MLOG_INFO(x1) mlog::write_message(mlog_level::info, x1)
#endif

#ifdef DISABLE_MLOG_WARNING
#define MLOG_WARNING(x1)
#else
#define MLOG_WARNING(x1) mlog::write_message(mlog_level::warning, x1)
#endif

#ifdef DISABLE_MLOG_ERROR
#define MLOG_ERROR(x1)
#else
#define MLOG_ERROR(x1) mlog::write_message(mlog_level::error, x1)
#endif

#ifdef DISABLE_MLOG_FATAL
#define MLOG_FATAL(x1)
#else
#define MLOG_FATAL(x1) mlog::write_message(mlog_level::fatal, x1)
#endif

#endif /* MLOG_HPP_ */

#ifdef MLOG_NO_LIB
#include "impl/mlog.hpp"
#endif
