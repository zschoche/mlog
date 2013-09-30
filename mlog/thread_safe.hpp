/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __THREAD_SAFE_HPP__
#define __THREAD_SAFE_HPP__

#if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/control/if.hpp>
#endif

#include <boost/detail/lightweight_mutex.hpp>
#include <boost/config.hpp>
#include "logger.hpp"

namespace mlog {

template <class logger_type> class thread_safe : public logger_type {
      public:

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

	template <typename... Args>
	thread_safe(Args &&... args)
	    : logger_type(std::forward<Args>(args)...) {}

#else

#define CTOR(z, n, unused)                                                     \
	BOOST_PP_IF(n, template <, ) BOOST_PP_ENUM_PARAMS(n, typename Arg)     \
		    BOOST_PP_IF(n, >, )                                        \
		    thread_safe(BOOST_PP_ENUM_BINARY_PARAMS(n, Arg, arg))      \
	    : logger_type(BOOST_PP_ENUM_PARAMS(n, arg)) {}

	BOOST_PP_REPEAT(5, CTOR, ~)
#undef CTOR

#endif // ! defined( BOOST_NO_CXX11_VARIADIC_TEMPLATES )

	virtual ~thread_safe() {}

	virtual void write_to_log(log_metadata &&metadata,
				  std::string &&log_text) {
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		logger_type::write_to_log(std::move(metadata),
					  std::move(log_text));
	}

      private:
	boost::detail::lightweight_mutex m_mutex;
};

} /* mlog */

#endif /* __THREAD_SAFE_HPP__ */
