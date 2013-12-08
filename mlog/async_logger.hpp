#ifndef ___ASYNC_LOGGER_HPP___
#define ___ASYNC_LOGGER_HPP___

#include "logger.hpp"
#include <boost/detail/lightweight_mutex.hpp>

#include <future>

namespace mlog {
template <typename T> class async_logger : public T {
      public:

	template <typename... Args>
	async_logger(Args &&... args)
	    : T(std::forward<Args>(args)...) {}

	virtual ~async_logger() {
		if (m_result.valid())
			m_result.wait();
	}

	void write_to_log(log_metadata &&metadata, std::string &&log_text) {
		if (m_result.valid())
			m_result.wait();
		m_result = std::async([&](log_metadata && md,
			      std::string && text) {
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
