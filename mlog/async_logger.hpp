#ifndef ___ASYNC_LOGGER_HPP___
#define ___ASYNC_LOGGER_HPP___

#if defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/control/if.hpp>
#endif

#include <boost/detail/lightweight_mutex.hpp>
#include <boost/config.hpp>
#include <future>
#include "logger.hpp"

namespace mlog {
template <typename T> class async_logger : public T {
      public:

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

	template <typename... Args>
	async_logger(Args &&... args)
	    : T(std::forward<Args>(args)...) {}

#else

#define CTOR(z, n, unused)                                                     \
	BOOST_PP_IF(n, template <, ) BOOST_PP_ENUM_PARAMS(n, typename Arg)     \
		    BOOST_PP_IF(n, >, )                                        \
		    async_logger(BOOST_PP_ENUM_BINARY_PARAMS(n, Arg, arg))      \
	    : T(BOOST_PP_ENUM_PARAMS(n, arg)) {}

	BOOST_PP_REPEAT(5, CTOR, ~)
#undef CTOR

#endif // ! defined( BOOST_NO_CXX11_VARIADIC_TEMPLATES )
	virtual ~async_logger() {
		if (m_result.valid())
			m_result.wait();
	}

	void write_to_log(log_metadata &&metadata, std::string &&log_text) {
		if (m_result.valid())
			m_result.wait();
		m_result = std::async([&](log_metadata md,
			      std::string text) {
				T::write_to_log(std::move(md),
							std::move(text));
			   },
			   std::move(metadata), std::move(log_text));
	}

      private:
	std::future<void> m_result;
};

} /*  mlog */

#endif
