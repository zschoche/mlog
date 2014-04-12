/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __MEMORY_LOGGER_HPP__
#define __MEMORY_LOGGER_HPP__

#include "logger.hpp"
#include <atomic>
#include <stdexcept>
#include <boost/detail/lightweight_mutex.hpp>
#include <cmath>
#include <sstream>

namespace mlog {

struct memory_entry {
	log_metadata metadata;
	std::string text;
};

#ifdef _MSC_VER
unsigned long log2(unsigned long x, unsigned long count = 0)
#else
    constexpr unsigned long log2(unsigned long x, unsigned long count = 0)
#endif
{
	return x < 2 ? count : log2((x >> 1), count + 1);
}

// writing is already thread-safe.
template <unsigned long N> class memory_logger : public logger<memory_logger<N> > {
      public:

	memory_logger() : logger<memory_logger<N> >(), m_size(N), m_offset(0), m_use_mutex(false)  {
#ifdef _MSC_VER
		unsigned long bits = mlog::log2(N - 1) + 1;
		unsigned long mask = ((1 << bits) - 1);
		if ((N - 1) != mask)
			throw std::invalid_argument("N is not valid. Try 4, 8, "
						    "16, 32 ... 1024, 2048, "
						    "4096..");
#else
		constexpr unsigned long bits = mlog::log2(N - 1) + 1;
		constexpr unsigned long mask = ((1 << bits) - 1);
		static_assert((N - 1) == mask, "N is not valid. Try 4, 8, 16, "
					       "32 ... 1024, 2048, 4096... and "
					       "so on.");
#endif
	}

	inline bool is_lock_free() {
		return m_use_mutex.is_lock_free() && m_offset.is_lock_free();
	}

	virtual ~memory_logger() {}

	void write_to_log(log_metadata &&metadata,
			  std::string &&log_text) {

		std::size_t offset = get_offset();
		memory_entry& entry = m_log_entrys[offset];
		//entry.metadata = std::move(metadata); //odd errors on clang 
		//entry.text = std::move(log_text); //odd errors on clang
		entry.metadata = metadata;
		entry.text = log_text;
	}
	
		
	void write_to_log(const log_metadata& metadata,
				  const std::string& log_text) {

		std::size_t offset = get_offset();
		memory_entry& entry = m_log_entrys[offset];
		entry.metadata = metadata;
		entry.text = log_text;
	}

	std::ostream &output(std::ostream &stream) {
		m_offset_temp = 0;
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		m_use_mutex = false;
		std::size_t offset = m_offset & (N - 1);

		for (unsigned long index = offset; index < N; index++)
			if(!m_log_entrys[index].text.empty()) stream << m_log_entrys[index].metadata.to_string(m_log_entrys[index].text, true);

		for (unsigned long index = 0; index < offset; index++)
			if(!m_log_entrys[index].text.empty()) stream << m_log_entrys[index].metadata.to_string(m_log_entrys[index].text, true);
		return stream;
	}

	std::string to_string() {
		std::stringstream ss;
		output(ss);
		std::string result = ss.str();
		return ss.str();

	}

	const memory_entry &operator[](std::size_t const &index) const {
		return m_log_entrys[(m_offset + index) & N_1];
	}

	inline unsigned long size() const {
		return m_size; 
	}

      private:

	inline unsigned long get_offset() {
		if(m_use_mutex) {
			auto r = m_offset + m_offset_temp.fetch_add(1) & N_1;
			boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
			return r;
		} else
			return  m_offset.fetch_add(1) & N_1;
	}

	const unsigned long m_size;

	static const unsigned long N_1 = N - 1;

	memory_entry m_log_entrys[N];
	std::atomic<unsigned long> m_offset;
	std::atomic<unsigned long> m_offset_temp;
	std::atomic<bool> m_use_mutex;
	boost::detail::lightweight_mutex m_mutex;
};

typedef memory_logger<4096> memory_logger_normal;
typedef memory_logger<static_cast<unsigned short>(-1)> memory_logger_big;

							       // is already
							       // thread-safe.
							 // already thread-safe.

#ifdef _MSC_VER
template <unsigned long N>
std::ostream &operator<<(std::ostream &lhs, memory_logger<N> &rhs)
#else
    template <unsigned long N>
constexpr std::ostream &operator<<(std::ostream &lhs, memory_logger<N> &rhs)
#endif
{
	return rhs.output(lhs);
}

} /* mlog */

#endif /* __MEMORY_LOGGER_HPP__ */
