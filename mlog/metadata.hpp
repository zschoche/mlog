/*
 * logger.hpp
 *
 *  Created on: Aug 9, 2012
 *      Author: philipp
 */

#ifndef __METADATA_HPP__
#define __METADATA_HPP__

#include <string>
#if defined(_GLIBCXX_HAS_GTHREADS) || defined(__clang__) ||                    \
    (defined(_MSC_VER) && (_MSC_VER >= 1700))
#include <thread>
#define THREAD_GET_ID() std::this_thread::get_id()
#else
#include <boost/thread.hpp>

namespace std {
using boost::mutex;
using boost::recursive_mutex;
using boost::lock_guard;
using boost::condition_variable;
using boost::unique_lock;
using boost::thread;
}
#define THREAD_GET_ID() boost::this_thread::get_id()
#endif
#include <chrono>
#include <cstdlib>
#include "mlog.hpp"

#include <sstream>
#include <cstdio>

enum mlog_level {
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

namespace mlog {
	
	
template <typename T> std::string thread_id_to_string(T &&thread_id) {
	std::stringstream ss;
	ss << thread_id;
	return ss.str();
}

// This should not be cleaned up https://github.com/zschoche/mlog/issues/11
extern mlog_manager *manager;

template <typename T> inline std::string level_to_string(T &&level) {
	if (level == mlog_level::trace)
		return "trace";
	else if (level == mlog_level::debug)
		return "debug";
	else if (level == mlog_level::info)
		return "info";
	else if (level == mlog_level::warning)
		return "warning";
	else if (level == mlog_level::error)
		return "error";
	else
		return "fatal";
}

struct log_position {
	log_position() : filename(), line_number(0) {}

	log_position(std::string _filename, std::size_t _line_number)
	    : filename(std::move(_filename)),
	      line_number(std::move(_line_number)) {}

	std::string filename;
	std::size_t line_number;

	inline bool has_value() const { return line_number != 0; }
};

struct log_metadata {

	// typedef std::chrono::high_resolution_clock clocks;
	typedef std::chrono::system_clock clocks;

	mlog_level level;
	std::chrono::time_point<clocks> time;
	std::thread::id thread_id;
	log_position position;

	log_metadata(const log_metadata &) = default;
	log_metadata(log_metadata &&) = default;
	log_metadata &operator=(const log_metadata &) = default;
	log_metadata &operator=(log_metadata &&) = default;
	~log_metadata() = default;

	log_metadata() : level(info) {}

	static inline std::chrono::time_point<clocks> get_time() {
		if (mlog::manager->use_time())
			return clocks::now();
		else
			return std::chrono::time_point<clocks>();
	}

	static inline std::thread::id get_thread_id() {
		if (manager->use_thread_id()) {
			return THREAD_GET_ID();
		} else
			return std::thread::id();
	}

	log_metadata(mlog_level &&lvl)
	    : level(std::move(lvl)), time(get_time()),
	      thread_id(get_thread_id()) {}

	log_metadata(mlog_level &&lvl, log_position &&_position)
	    : level(std::move(lvl)), time(get_time()),
	      thread_id(get_thread_id()), position(_position) {}

	log_metadata(mlog_level &&lvl, const log_position &_position)
	    : level(std::move(lvl)), time(get_time()),
	      thread_id(get_thread_id()), position(std::move(_position)) {}

	std::string to_string(const std::string &end_string = std::string(),
			      bool end_line = false) const;

};
}
#endif /* __METADATA_HPP_ */
