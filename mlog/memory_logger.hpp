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

namespace mlog
{

struct memory_entry
{
	log_metadata metadata;
	std::string text;
};

constexpr unsigned long log2(unsigned long x, unsigned long count = 0)
{
	return x < 2 ? count : log2((x>>1), count + 1);
}


// writing is already thread-safe.
template<unsigned long N>
class memory_logger : public logger
{
public:

	memory_logger()
	:logger(),
	m_size(N),
	m_use_mutex(false)
	{
		constexpr unsigned long bits = mlog::log2(N - 1)+1;
		constexpr unsigned long mask =  ((1 << bits) - 1);
		static_assert((N -1) == mask, "N is not valid. Try 4, 8, 16, 32 ... 1024, 2048, 4096... and so on.");
		/*if((N -1) != mask) 
		{
			std::stringstream ss;
			ss << "Invalid template argument mlog::memory_logger::N. Next valid entry count is " << mask+1 << "."; 
			throw std::invalid_argument(ss.str());
		}*/
		
		
	}

	inline bool is_lock_free()
	{
		return m_use_mutex.is_lock_free() && m_offset.is_lock_free();
	}


	virtual ~memory_logger()
	{
	}

	void write_to_log(log_metadata&& metadata, std::string&& log_text) override
	{
	
		if(m_use_mutex)
		{
			std::size_t index = (m_offset + m_offset_temp++)  & (N - 1);
			boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
			memory_entry& entry = m_log_entrys[index];
			entry.metadata = std::move(metadata);
			entry.text = std::move(log_text);
		}
		else
		{
			memory_entry& entry = m_log_entrys[m_offset++ & (N - 1)];
			entry.metadata = std::move(metadata);
			entry.text = std::move(log_text);
		}
	}
	

	std::ostream& output(std::ostream& stream) 
	{
		m_offset_temp = 0;
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		m_use_mutex = false;
		std::size_t offset = m_offset & (N - 1);
		
		for(unsigned long index = offset; index < N; index++)
			m_log_entrys[index].metadata.output(stream) << m_log_entrys[index].text << std::endl;

		for(unsigned long index = 0; index < offset; index++)
			m_log_entrys[index].metadata.output(stream) << m_log_entrys[index].text << std::endl;

		return stream;
	}

	const memory_entry& operator[](std::size_t const& index) const
	{
		return m_log_entrys[(m_offset + index) & (N - 1)];
	}

private:

	const unsigned long m_size;

	
	//const unsigned long m_size = N;

	memory_entry m_log_entrys[N];
	std::atomic<unsigned long> m_offset;
	std::atomic<unsigned long> m_offset_temp;
	std::atomic<bool> m_use_mutex;
	boost::detail::lightweight_mutex m_mutex;

};

typedef memory_logger<4096> memory_logger_normal;
typedef memory_logger<static_cast<unsigned short>(-1)> memory_logger_big;

typedef memory_logger_normal memory_logger_normal_thread_safe; //memory_logger is already thread-safe.
typedef memory_logger_big memory_logger_big_thread_safe; //memory_logger is already thread-safe.

template<unsigned long N>
constexpr std::ostream& operator<<(std::ostream& lhs, memory_logger<N> & rhs) 
{ 
 	return rhs.output(lhs); 
}

} /* mlog */

#endif /* __MEMORY_LOGGER_HPP__ */
