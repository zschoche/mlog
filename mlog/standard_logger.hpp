/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __STANDARD_LOGGER_HPP__
#define __STANDARD_LOGGER_HPP__

#include "logger.hpp"
#include "thread_safe.hpp"
#include <iostream>

namespace mlog {

struct standard_logger : logger<standard_logger> {

	void flush() { std::cout.flush(); }

	template<typename M,typename T>
	void write_to_log(M&& metadata, T&& log_text) {
		//metadata.output(std::cout) << std::forward<T>(log_text) << std::endl;
		std::string str = metadata.to_string(std::forward<T>(log_text), true);
		std::fwrite(str.c_str(), str.size(), 1, stdout );
	}

};

typedef thread_safe<standard_logger> standard_logger_thread_safe;

} /* mlog */

#endif /* __STANDARD_LOGGER_HPP__ */
