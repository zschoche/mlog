/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __THREAD_SAFE_HPP__
#define __THREAD_SAFE_HPP__

#include <boost/detail/lightweight_mutex.hpp>
#include "logger.hpp"

namespace mlog 
{
	

template<class logger_type>
class thread_safe : public logger_type
{
public:

	//template <typename... Args>
	//thread_safe(Args&&... args)
	//:logger_type(std::forward<Args>(args)...)
	//{
	//}

	thread_safe()
	:logger_type()
	{
	}

	template <typename Arg>
	thread_safe(Arg &&arg)
	:logger_type(std::forward<Arg>(arg))
	{
	}

	template <typename Arg1, typename Arg2>
	thread_safe(Arg1 &&arg1, Arg2 &&arg2)
	:logger_type(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2))
	{
	}

	virtual ~thread_safe()
	{
	}

	virtual void write_to_log(log_metadata&& metadata, std::string&& log_text)
	{
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		logger_type::write_to_log(std::move(metadata), std::move(log_text));
	}


private:
	boost::detail::lightweight_mutex m_mutex;


};


} /* mlog */

#endif /* __THREAD_SAFE_HPP__ */
