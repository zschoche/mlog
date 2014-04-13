/*
*
*	Author: Philipp Zschoche
*
*/

#ifndef __METADATA_IPP__
#define __METADATA_IPP__

#include "../metadata.hpp"
#include <cstdio>
#if _MSC_VER
#define snprintf _snprintf_s
#endif

#include <string>
#include <sstream>
#include <cstring>
#include <chrono>
#include <cstdlib>

namespace mlog {

	template <typename T> std::string thread_id_to_string(T &&thread_id) {
	std::stringstream ss;
	ss << thread_id;
	return ss.str();
}

	std::chrono::time_point<log_metadata::clocks> log_metadata::get_time() {
		if (mlog::manager->use_time())
			return clocks::now();
		else
			return std::chrono::time_point<log_metadata::clocks>();
	}

	std::thread::id log_metadata::get_thread_id() {
		if (manager->use_thread_id()) {
			return THREAD_GET_ID();
		} else
			return std::thread::id();
	}


log_metadata::log_metadata(mlog_level &&lvl)
	    : level(std::move(lvl)), time(get_time()),
	      thread_id(get_thread_id()) {}

log_metadata::log_metadata(mlog_level &&lvl, log_position &&_position)
	    : level(std::move(lvl)), time(get_time()),
	      thread_id(get_thread_id()), position(_position) {}

log_metadata::log_metadata(mlog_level &&lvl, const log_position &_position)
	    : level(std::move(lvl)), time(get_time()),
	      thread_id(get_thread_id()), position(std::move(_position)) {}


std::string log_metadata::to_string(const std::string &end_string,
				    bool end_line) const {

	static const std::size_t max_size = 512;
	std::string result;
	result.resize(max_size + end_string.size());

	char *buffer = const_cast<char *>(result.c_str());
	int len;
	if (manager->use_time()) {

		const std::time_t timet = clocks::to_time_t(time);


		unsigned int ms =
		    std::chrono::duration_cast<std::chrono::milliseconds>(
			time.time_since_epoch()).count() -
		    timet * 1000000;

		std::tm *tm = std::gmtime(&timet);
		static int current_hour = -1;
		static int local_factor = 0;

		if (tm->tm_hour != current_hour) {
			current_hour = tm->tm_hour;
			tm = std::localtime(&timet);
			local_factor = tm->tm_hour - current_hour;
		} else {
			tm->tm_hour += local_factor;
		}

		if (manager->use_thread_id()) {
			if (position.has_value()) // 2012-11-02 15:24:04.345
						  // [file:line_number
				// 24-0x7fff72ca8180]{warning}:
				len = snprintf(
				    buffer, max_size,
				    "%04i-%02i-%02i %02i:%02i:%02i.%u [%s:%i %02i-%s]{%s}: ",
				    1900 + tm->tm_year, tm->tm_mon, tm->tm_mday,
				    tm->tm_hour, tm->tm_min, tm->tm_sec, ms,
				    position.filename.c_str(),
				    position.line_number, manager->session(),
				    thread_id_to_string(thread_id).c_str(),
				    level_to_string(level).c_str());
			else // 2012-11-02 15:24:04.345
				// [24-0x7fff72ca8180]{warning}:
				len = snprintf(buffer, max_size,
					 "%04i-%02i-%02i %02i:%02i:%02i.%u "
					 "[%02i-%s]{%s}: ",
					 1900 + tm->tm_year, tm->tm_mon,
					 tm->tm_mday, tm->tm_hour, tm->tm_min,
					 tm->tm_sec, ms, manager->session(),
					 thread_id_to_string(thread_id).c_str(),
					 level_to_string(level).c_str());
		} else // 2012-11-02 15:24:04.345 [24]{warning}:
		{
			if (position.has_value()) {
				len = snprintf(
				    buffer, max_size,
				    "%04i-%02i-%02i %02i:%02i:%02i.%u[%s:%i "
				    "%02i]{%s}: ",
				    1900 + tm->tm_year, tm->tm_mon, tm->tm_mday,
				    tm->tm_hour, tm->tm_min, tm->tm_sec, ms,
				    position.filename.c_str(),
				    position.line_number, manager->session(),
				    level_to_string(level).c_str());
			} else {
				len = snprintf(buffer, max_size,
					 "%04i-%02i-%02i "
					 "%02i:%02i:%02i.%u[%02i]{%s}: ",
					 1900 + tm->tm_year, tm->tm_mon,
					 tm->tm_mday, tm->tm_hour, tm->tm_min,
					 tm->tm_sec, ms, manager->session(),
					 level_to_string(level).c_str());
			}
		}
	} else if (manager->use_thread_id()) //[24-0x7fff72ca8180]{warning}:
	{
		if (position.has_value()) {
			len = snprintf(buffer, max_size, "[%s:%i %02i-%s]{%s}: ",
				 position.filename.c_str(),
				 position.line_number, manager->session(),
				 thread_id_to_string(thread_id).c_str(),
				 level_to_string(level).c_str());
		} else {
			len = snprintf(buffer, max_size, "[%02i-%s]{%s}: ",
				 manager->session(),
				 thread_id_to_string(thread_id).c_str(),
				 level_to_string(level).c_str());
		}
	} else if (position.has_value()) {
		len = snprintf(buffer, max_size, "[%s:%i %02i]{%s}: ",
			 position.filename.c_str(), position.line_number,
			 manager->session(), level_to_string(level).c_str());
	} else //[24]{warning}:
	{
		 len = snprintf(buffer, max_size, "[%02i]{%s}: ", manager->session(),
			 level_to_string(level).c_str());
	}
	if(len > max_size) {
		len = max_size;
	}
	
	if (!end_string.empty()) {
		memcpy(&buffer[len], end_string.c_str(), end_string.size());
		len += end_string.size();
	}
	
	
	if (end_line) {
		result.resize(len + 1);
		//result[len] = '\r';
		result[len] = '\n';
	} else {
		result.resize(len);
	}
	return result;
}

} /* mlog */

#endif
