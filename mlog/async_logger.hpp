#ifndef ___ASYNC_LOGGER_HPP___
#define ___ASYNC_LOGGER_HPP___

#if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/control/if.hpp>
#endif

#include <boost/detail/lightweight_mutex.hpp>
#include <boost/config.hpp>
#include <boost/lockfree/queue.hpp>
#include "logger.hpp"

namespace mlog {
template <typename logger_type>
struct async_logger : logger<async_logger<logger_type> > {

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

	template <typename... Args>
	async_logger(Args &&... args)
	    : m_queue_size(1024), m_logger(std::forward<Args>(args)...),
	      m_worker(0), m_queue(m_queue_size) {}

	template <typename... Args>
	async_logger(std::size_t queue_size, Args &&... args)
	    : m_queue_size(queue_size), m_logger(std::forward<Args>(args)...),
	      m_worker(0), m_queue(m_queue_size) {}
	

#else

#define CTOR(z, n, unused)                                                     \
	BOOST_PP_IF(n, template <, ) BOOST_PP_ENUM_PARAMS(n, typename Arg)     \
		    BOOST_PP_IF(n, >, )                                        \
		    async_logger(BOOST_PP_ENUM_BINARY_PARAMS(n, Arg, arg))     \
	    : m_queue_size(1024), m_logger(BOOST_PP_ENUM_PARAMS(n, arg)),      \
	      m_worker(0), m_queue(m_queue_size) {}

	BOOST_PP_REPEAT(5, CTOR, ~)
#undef CTOR

#endif // ! defined( BOOST_NO_CXX11_VARIADIC_TEMPLATES )

	typedef std::pair<log_metadata, std::string> queue_item;

	~async_logger() { flush(); }

	template <typename M, typename T>
	void write_to_log(M &&metadata, T &&log_text) {
		queue_item *item = new queue_item(std::forward<M>(metadata),
						  std::forward<T>(log_text));
		while (!m_queue.push(item))
			;
		if (m_worker == 0) {
			boost::detail::lightweight_mutex::scoped_lock lock(
			    m_mutex);
			if (m_worker == 0) {
				m_worker++;
				std::thread(&async_logger<logger_type>::worker,
					    this).detach();
			}
		}
	}

	inline logger_type &get() { return m_logger; }

	void flush() {
		while (m_worker > 0) {
			boost::detail::lightweight_mutex::scoped_lock lock(
			    m_worker_mutex);
		}
	}

	inline std::size_t queue_size() const { return m_queue_size; }

      private:

	const std::size_t m_queue_size;

	logger_type m_logger;
	std::atomic<unsigned int> m_worker;
	boost::lockfree::queue<queue_item *> m_queue;

	boost::detail::lightweight_mutex m_mutex;
	boost::detail::lightweight_mutex m_worker_mutex;
	// mlog::queue<queue_item> m_queue;

	void worker() {
		boost::detail::lightweight_mutex::scoped_lock lock(
		    m_worker_mutex);
		dowork();
		set_working_false();
		dowork();
	}

	void set_working_false() {
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		m_worker--;
	}

	void dowork() {
		queue_item *item;
		while (m_queue.pop(item)) {
			m_logger.write_to_log(item->first, item->second);
			delete item;
		}
	}
};

} /*  mlog */

#endif
