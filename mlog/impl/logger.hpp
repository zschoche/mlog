/*
*
*	Author: Philipp Zschoche
*
*/

#ifndef __LOGGER_IPP__
#define __LOGGER_IPP__

#include "../logger.hpp"
#include <boost/random.hpp>

#include <cstdio>
#if _MSC_VER
#define snprintf _snprintf_s
#endif

namespace mlog
{

log_metadata::log_metadata(mlog_level&& lvl, short session_id, bool _use_time, bool _use_thread_id)
:use_time(std::move(_use_time)), 
use_thread_id(std::move(_use_thread_id)),
level(std::move(lvl)),
session_id(std::move(session_id))
{
	if(use_time)
		time = clocks::now(); 

	if(use_thread_id)
		thread_id = std::this_thread::get_id();
}

std::string log_metadata::to_string() const
{
	char buffer[64];
	if(use_time)
	{

		std::time_t timet = clocks::to_time_t(time);

		uint64_t ms = std::chrono::duration_cast<std::chrono::nanoseconds>(time.time_since_epoch()).count() - timet * 1000000000;

#if _MSC_VER
		tm tmm;
		localtime_s(&tmm, &timet);
		tm* tm = &tmm;
#else
		std::tm* tm =  std::localtime(&timet);
#endif
		
		if(use_thread_id) //2012-11-02 15:24:04.345 [24-0x7fff72ca8180]{warning}: 
			snprintf(buffer, sizeof(buffer), "%04i-%02i-%02i %02i:%02i:%02i.%llu [%02i-%s]{%s}: ", 1900 + tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, ms, session_id, boost::lexical_cast<std::string>(thread_id).c_str(), level_to_string(level).c_str()); 
		else //2012-11-02 15:24:04.345 [24]{warning}: 
			snprintf(buffer, sizeof(buffer), "%04i-%02i-%02i %02i:%02i:%02i.%llu[%02i]{%s}: ", 1900 + tm->tm_year, tm->tm_mon, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, ms, session_id, level_to_string(level).c_str()); 
	}
	else if(use_thread_id)	//[24-0x7fff72ca8180]{warning}: 
		snprintf(buffer, sizeof(buffer), "[%02i-%s]{%s}: ", session_id, boost::lexical_cast<std::string>(thread_id).c_str(), level_to_string(level).c_str()); 
	else //[24]{warning}: 
		snprintf(buffer, sizeof(buffer), "[%02i]{%s}: ", session_id, level_to_string(level).c_str()); 

	return std::string(buffer); 
}

std::ostream& log_metadata::output(std::ostream& stream) const 
{
	stream << to_string();
	return stream; 
}

logger::logger()
:m_use_time(false),
m_use_thread_id(false)
{
	boost::random::mt19937 rng(std::time(0));
	boost::random::uniform_int_distribution<> six(0, 100);
	m_session = six(rng); 
}

logger::~logger()
{
}


	
	
} /* mlog */


#endif







