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


// writing is already thread-safe.
template<unsigned long N>
class memory_logger : public logger
{
public:

	memory_logger()
	:m_use_mutex(false)
	{
		//check size
		unsigned long bits = static_cast<unsigned long>(::log(m_size - 1) / ::log(2))+1;
		unsigned long mask =  (1 << bits) - 1;
		if((m_size -1) != mask) 
		{
			std::stringstream ss;
			ss << "Invalid template argument mlog::memory_logger::N. Next valid entry count is " << mask+1 << "."; 
			throw std::invalid_argument(ss.str());
		}
		
		if(!m_use_mutex.is_lock_free())
			throw std::runtime_error("std::atomic<bool> is not lock free.");
		
		if(!m_offset.is_lock_free())
			throw std::runtime_error("std::atomic<unsigned long> is not lock free.");
	}

	virtual ~memory_logger()
	{
	}

	virtual void write_to_log(log_metadata&& metadata, std::string&& log_text)
	{
	
		if(m_use_mutex)
		{
			std::size_t index = (m_offset + m_offset_temp++)  & (m_size - 1);
			boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
			memory_entry& entry = m_log_entrys[index];
			entry.metadata = std::move(metadata);
			entry.text = std::move(log_text);
		}
		else
		{
			memory_entry& entry = m_log_entrys[m_offset++ & (m_size - 1)];
			entry.metadata = std::move(metadata);
			entry.text = std::move(log_text);
		}
	}
	

	std::ostream& output(std::ostream& stream) 
	{
		m_offset_temp = 0;
		boost::detail::lightweight_mutex::scoped_lock lock(m_mutex);
		m_use_mutex = false;
		std::size_t offset = m_offset & (m_size - 1);
		
		for(unsigned long index = offset; index < m_size; index++)
			m_log_entrys[index].output(stream) << std::endl;

		for(unsigned long index = 0; index < offset; index++)
			m_log_entrys[index].output(stream) << std::endl;

		return stream;
	}

private:
	
	const unsigned long m_size = N;

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
std::ostream& operator<<(std::ostream& lhs, memory_logger<N> & rhs) 
{ 
 	return rhs.output(lhs); 
}

} /* mlog */

#endif /* __MEMORY_LOGGER_HPP__ */
