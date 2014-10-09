/*

 * mlog.hpp

 *

 *  Created on: Aug 12, 2012

 *      Author: philipp

 */

#ifndef _MLOG_MANAGER_HPP_
#define _MLOG_MANAGER_HPP_

#include <atomic>
#include <memory>

namespace mlog {

struct standard_logger;
struct logger_base;

struct mlog_manager {

	mlog_manager();
	~mlog_manager() {
		// This should not be cleaned up
		// https://github.com/zschoche/mlog/issues/11
	}

	inline logger_base *log() { return m_log; }

	void set_log(logger_base &log) { set_log(&log); }

	void set_log(logger_base *log);

	void unset_log() { this->set_log(nullptr); }

	template <typename T> inline void use_thread_id(T &&value) {
		m_use_thread_id = std::forward<T>(value);
	}

	inline bool use_thread_id() const { return m_use_thread_id; }

	template <typename T> inline void use_time(T &&value) {
		m_use_time = std::forward<T>(value);
	}

	inline bool use_time() const { return m_use_time; }

	template <typename T> inline void use_position(T &&value) {
		m_use_position = std::forward<T>(value);
	}

	inline bool use_position() const { return m_use_position; }
	inline short session() const { return m_session; }
	inline void set_default_settings() {
		use_time(false);
		use_thread_id(false);
		use_position(false);
	}

      private:
	std::atomic<logger_base *> m_log;
	std::atomic<bool> m_is_valid;

	short m_session;
	bool m_use_time;
	bool m_use_thread_id;
	bool m_use_position;
};
} /* namespace mlog */
#endif /* _MLOG_MANAGER_HPP_ */
