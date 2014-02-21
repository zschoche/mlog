/*
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
#include <atomic>
#include <memory>

namespace mlog {

struct mlog_manager {

		mlog_manager() : m_log(new standard_logger()), m_is_valid(false) {}
		~mlog_manager() {
			// This should not be cleaned up https://github.com/zschoche/mlog/issues/11
		}

		inline logger* log() {
			return m_log;
		}

		void set_log(logger& log) {
			set_log(&log);
		}

		void set_log(logger* log) {
			logger* old_log = m_log;

			if(log == nullptr) {
				m_log = new standard_logger();
			} else {
				m_log = log;
			}

			if(!m_is_valid)
				delete old_log;
			m_is_valid = log != nullptr;
		}

		void unset_log() {
			this->set_log(nullptr);
		}

	private:
		std::atomic<logger*> m_log;
		std::atomic<bool> m_is_valid;


};


// This should not be cleaned up https://github.com/zschoche/mlog/issues/11
extern mlog_manager* manager;

}; /* namespace mlog */

#ifdef MLOGTRACE
#define MLOG_TRACE(x1)                                                         \
	::mlog::manager->log()->write(mlog_level::trace, x1,                          \
			       ::mlog::log_position(__FILE__, __LINE__))

#else
#define MLOG_TRACE(x1)
#endif

#ifdef MLOGDEBUG
#define MLOG_DEBUG(x1)                                                         \
	::mlog::manager->log()->write(mlog_level::debug, x1,                          \
			       ::mlog::log_position(__FILE__, __LINE__))
#else
#define MLOG_DEBUG(x1)
#endif

#define MLOG_INFO(x1)                                                          \
	::mlog::manager->log()->write(mlog_level::info, x1,                           \
			       ::mlog::log_position(__FILE__, __LINE__))

#define MLOG_WARNING(x1)                                                       \
	::mlog::manager->log()->write(mlog_level::warning, x1,                        \
			       ::mlog::log_position(__FILE__, __LINE__))

#define MLOG_ERROR(x1)                                                         \
	::mlog::manager->log()->write(mlog_level::error, x1,                          \
			       ::mlog::log_position(__FILE__, __LINE__))

#define MLOG_FATAL(x1)                                                         \
	::mlog::manager->log()->write(mlog_level::fatal, x1,                          \
			       ::mlog::log_position(__FILE__, __LINE__))

#endif /* MLOG_HPP_ */

#ifdef MLOG_NO_LIB
#include "impl/mlog.hpp"
#endif
