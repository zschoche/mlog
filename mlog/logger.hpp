/*
 * logger.hpp
 *
 *  Created on: Aug 9, 2012
 *      Author: philipp
 */

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__


#include <string>
#include <thread>
#include <chrono>

enum mlog_level
{
	trace, debug, info, warning, error, fatal
};

namespace mlog
{

template<typename T>
inline std::string level_to_string(T&& level)
{
	if(level == mlog_level::trace)
		return "trace";
	else if(level == mlog_level::debug)
		return "debug";
	else if(level == mlog_level::info)
		return "info";
	else if(level == mlog_level::warning)
		return "warning";
	else if(level == mlog_level::error)
		return "error";
	else
		return "fatal";
}

struct log_metadata
{

	//typedef std::chrono::high_resolution_clock clocks;
	typedef std::chrono::system_clock clocks;

	bool use_time;
	bool use_thread_id;
	mlog_level level;
	short session_id;
	std::chrono::time_point<clocks> time;
	std::thread::id thread_id;

	log_metadata();

	log_metadata(mlog_level&& lvl, short session_id, bool _use_time, bool _use_thread_id);
	
	std::string to_string() const;

	std::ostream& output(std::ostream& stream) const;
};

class logger
{
public:
	logger();
	
	virtual ~logger();
		

	inline void write(mlog_level&& level, std::string&& log_text)
	{
		write_to_log(log_metadata(std::move(level), m_session, m_use_time, m_use_thread_id), std::move(log_text));
	}

	inline void write(mlog_level&& level, const std::string& log_text)
	{
		write_to_log(log_metadata(std::move(level), m_session, m_use_time, m_use_thread_id), std::string(log_text));
	}

	//virtual void flush() = 0;
	virtual void write_to_log(log_metadata&& metadata, std::string&& log_text) = 0;

	
	inline void use_thread_id(bool value) 
	{
		m_use_thread_id = value;
	}
	
	inline bool use_thread_id() const
	{
		return m_use_thread_id;
	}


	inline void use_time(bool value) 
	{
		m_use_time = value;
	}
	
	inline bool use_time() const
	{
		return m_use_time;
	}

private:
	short m_session;
	bool m_use_time;
	bool m_use_thread_id;
};



}
#endif /* LOGGER_HPP_ */

#ifdef MLOG_NO_LIB
#include "impl/logger.ipp"
#endif
