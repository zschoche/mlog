/*
 * logger.hpp
 *
 *  Created on: Aug 9, 2012
 *      Author: philipp
 */

#ifndef __METADATA_HPP__
#define __METADATA_HPP__

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


enum mlog_level {
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

namespace mlog {
	
	


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

	log_position(const char* _file, std::size_t _line_number)
	    : filename(cut_filename(_file)),
	      line_number(std::move(_line_number)) {}

	std::string filename;
	int line_number;

	inline static const char* cut_filename(const char* file) {
		const char* result = file;
		const char* ptr = file;
		while(*ptr != '\0') {
			if(*ptr == separator()) {
				ptr++;
				result = ptr;
			} else {
				ptr++;
			}
		}
		if(ptr == result) {
			return file;
		} else { 
			if (ptr - result > 256) {
				return ptr - 256;
			} else {	
				return result;
			}
		}
	}

	constexpr static char separator() {
#if defined(_WIN32_WCE) || defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
		return '\\';
#else
		return '/';
#endif
	}

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

	static inline std::chrono::time_point<clocks> get_time(); 
	static inline std::thread::id get_thread_id();

	log_metadata(mlog_level &&lvl);
	log_metadata(mlog_level &&lvl, log_position &&_position);
	log_metadata(mlog_level &&lvl, const log_position &_position);
	std::string to_string(const std::string &end_string = std::string(),
			      bool end_line = false) const;

};
}
#endif /* __METADATA_HPP_ */
