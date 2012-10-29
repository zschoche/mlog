/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __THREAD_SAFE_HPP__
#define __THREAD_SAFE_HPP__

#include <boost/move/move.hpp>
#include <boost/thread.hpp>
#include <boost/detail/lightweight_mutex.hpp>


namespace mlog 
{
	

template<class logger_type, class mutex_type>
class synchronize : public logger_type
{
public:

	template <typename... Args>
	synchronize(Args... args)
	:logger_type(boost::forward<Args>(args)...)
	{
	}

	virtual ~thread_safe()
	{
	}

	virtual void write_to_log(const std::string& log_text)
	{
		mutex_type::scoped_lock lock(m_mutex);
		logger_type::write_to_log(log_text);
	}


private:
	boost::detail::lightweight_mutex m_mutex;

};

template<class logger_type>
typedef synchronize<logger_type, boost::detail::lightweight_mutex> thread_safe;





} /* mlog */

#endif /* __THREAD_SAFE_HPP__ */
