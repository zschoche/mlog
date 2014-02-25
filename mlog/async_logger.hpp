#ifndef ___ASYNC_LOGGER_HPP___
#define ___ASYNC_LOGGER_HPP___

#if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/control/if.hpp>
#endif

#include <boost/detail/lightweight_mutex.hpp>
#include <boost/thread/locks.hpp> 
#include <boost/thread/lock_guard.hpp> 
#include <boost/config.hpp>
#include <future>
#include <boost/lockfree/queue.hpp>
#include "queue.hpp"
#include "logger.hpp"

namespace mlog {
template <typename logger_type> 
struct async_logger : logger<async_logger<logger_type> > {


#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

	template <typename... Args>
	async_logger(Args &&... args)
	    : m_logger(std::forward<Args>(args)...),m_working(false), m_done(false) {
	    }

#else

#define CTOR(z, n, unused)                                                     \
	BOOST_PP_IF(n, template <, ) BOOST_PP_ENUM_PARAMS(n, typename Arg)     \
		    BOOST_PP_IF(n, >, )                                        \
		    async_logger(BOOST_PP_ENUM_BINARY_PARAMS(n, Arg, arg))      \
	    : m_logger(BOOST_PP_ENUM_PARAMS(n, arg)), m_working(false), m_done(false) {}

	BOOST_PP_REPEAT(5, CTOR, ~)
#undef CTOR

#endif // ! defined( BOOST_NO_CXX11_VARIADIC_TEMPLATES )


	typedef std::pair<log_metadata, std::string> queue_item;

	~async_logger() {
		m_done = true;
		boost::detail::lightweight_mutex::scoped_lock lock(m_worker_mutex);
	}

	template<typename M, typename T>
	void write_to_log(M&& metadata, T&& log_text) {
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		m_queue.push(queue_item(std::forward<M>(metadata), std::forward<T>(log_text)));
		if(!m_working) {
			m_working = true;
			std::thread([&] { worker(); }).detach();
		}
	}
	
	inline logger_type& get() {
		return m_logger;
	}

      private:
	logger_type m_logger;
	std::atomic<bool> m_working;
	std::atomic<bool> m_done;

	boost::detail::lightweight_mutex m_mutex;
	boost::detail::lightweight_mutex m_worker_mutex;
	mlog::queue<queue_item> m_queue;

	void worker() {
		boost::detail::lightweight_mutex::scoped_lock lock(m_worker_mutex);
		if(!m_done) {
			dowork();
			set_working_false();
			dowork();
		}
	}
	void set_working_false() {
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		m_working = false;
	}

	void dowork() {
		queue_item item;
		while(m_queue.pop(item)) {
			m_logger.write_to_log(item.first, item.second);
		}
	}


};

} /*  mlog */

#endif
