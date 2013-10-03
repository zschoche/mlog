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

class standard_logger : public logger {
      public:

	standard_logger() : logger() {}

	virtual ~standard_logger() {}

	void flush() { std::cout.flush(); }

	void write_to_log(log_metadata &&metadata,
			  std::string &&log_text) {
		metadata.output(std::cout) << log_text << std::endl;
	}

      private:
};

typedef thread_safe<standard_logger> standard_logger_thread_safe;

} /* mlog */

#endif /* __STANDARD_LOGGER_HPP__ */
