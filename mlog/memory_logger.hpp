/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __MEMORY_LOGGER_HPP__
#define __MEMORY_LOGGER_HPP__

#include "logger.hpp"
#include <string>
#include <atomic>
#include <stdexcept>
#include <iostream>

namespace mlog
{
// writing is already thread-safe.
template<unsigned long N>
class memory_logger : public logger
{
public:

	memory_logger()
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

	}

	virtual ~memory_logger()
	{

	}

	virtual void write_to_log(const std::string& log_text)
	{
		std::size_t index = m_offset++ & (m_size - 1);
		m_log_entrys[index] = log_text;
	}

	
	std::ostream& output(std::ostream& stream) const 
	{
		std::size_t offset = m_offset & (m_size - 1);
		
		for(unsigned long index = offset; index < m_size; index++)
			stream << m_log_entrys[index] << std::endl;
		

		for(unsigned long index = 0; index < offset; index++)
			stream << m_log_entrys[index] << std::endl;

		return stream;
	}

	

private:
	 
	
	const unsigned long m_size = N;

	std::string m_log_entrys[N];
	std::atomic<unsigned long> m_offset;

};

typedef memory_logger<128> memory_logger_small;
typedef memory_logger<4096> memory_logger_normal;
typedef memory_logger<static_cast<unsigned short>(-1)> memory_logger_medium;
typedef memory_logger<static_cast<unsigned int>(-1)> memory_logger_big;


template<unsigned long N>
std::ostream& operator<<(std::ostream& lhs, memory_logger<N> const& rhs) 
{ 
 	return rhs.output(lhs); 
}

} /* mlog */

#endif /* __MEMORY_LOGGER_HPP__ */
