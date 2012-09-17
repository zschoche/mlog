/*
 * logger.hpp
 *
 *  Created on: Aug 9, 2012
 *      Author: philipp
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_


#include <string>
#include <sstream>
#include <iostream>
#include <boost/random.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread.hpp>

enum mlog_level
{
	trace, debug, info, warning, error, fatal
};

namespace mlog
{


class logger
{
public:
	logger();

	virtual ~logger();

	void write(const std::string& log_text);
	
	virtual void flush();
	virtual void write_to_log(const std::string& log_text);
private:
	int m_session;
};



}
#endif /* LOGGER_HPP_ */
