/*
 * leveled_logger.hpp
 *
 *  Created on: Aug 12, 2012
 *      Author: philipp
 */

#ifndef __LEVELED_LOGGER_HPP__
#define __LEVELED_LOGGER_HPP__

#include "logger.hpp"
#include <sstream>

namespace mlog
{

extern char endl;

class leveled_logger
{
public:
	leveled_logger(mlog_level level, logger* log) :
			m_log(log), m_has_content(false), m_level(level)
	{
	}

	virtual ~leveled_logger()
	{
		if (m_has_content)
			write_to_log();

	}

	template<typename T>
	leveled_logger& operator<<(T rhs)
	{
		//if (!m_has_content)
		//	init_line();
		m_has_content = true;
		m_stream << rhs;
		return *this;
	}

	leveled_logger& operator<<(char rhs)
	{
		m_has_content = true;

		if (rhs == mlog::endl)
			write_to_log();
		else
			m_stream << rhs;

		return *this;
	}

	inline mlog_level level() const
	{
		return m_level;
	}

protected:
	inline logger* get_logger()
	{
		return m_log;
	}

private:
	std::stringstream m_stream;
	logger* m_log;
	bool m_has_content;
	mlog_level m_level;
	

	void write_to_log()
	{
		m_log->write(level(), m_stream.str()); 
		m_has_content = false;
	}

};

} /* namespace mlog */
#endif /* LEVELED_LOGGER_HPP_ */

#ifdef MLOG_NO_LIB
#include "impl/leveled_logger.hpp"
#endif

