/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __THREAD_SAFE_HPP__
#define __THREAD_SAFE_HPP__

#include <boost/move/move.hpp>
#include <boost/thread.hpp>

namespace mlog 
{
	

template<class logger_type>
class thread_safe : public logger_type
{
public:

	template <typename... Args>
	thread_safe(Args... args)
	:logger_type(boost::forward<Args>(args)...)
	{
	}

	virtual ~thread_safe()
	{
	}

	virtual void write_to_log(const std::string& log_text)
	{
		boost::mutex::scoped_lock lock(m_mutex);
		logger_type::write_to_log(log_text);
	}


private:
	boost::mutex m_mutex;
};

} /* mlog */

#endif /* __THREAD_SAFE_HPP__ */
