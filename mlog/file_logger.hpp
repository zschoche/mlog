/*
*
*	Author: Philipp Zschoche
*
*/
#ifndef __FILE_LOGGER_HPP__
#define __FILE_LOGGER_HPP__

#include "logger.hpp"
#include <string>
#include <boost/iostreams/device/file.hpp>
#include <boost/filesystem.hpp>
#include <boost/shared_ptr.hpp>

namespace mlog 
{

class file_logger : public logger  
{
public:
	
	file_logger(const std::string& log_name, const std::string& log_directory = ".", std::size_t max_file_size = 0);
	virtual ~file_logger();
	virtual void write_to_log(const std::string& log_text); 

	inline void max_file_size(std::size_t value) 
	{
		m_max_file_size = value;
	}
		
	inline std::size_t max_file_size() const
	{
		return m_max_file_size;
	}

	virtual void flush();
private:
	std::string m_log_name;		
	std::string m_log_directory;
	std::size_t m_max_file_size;
	std::size_t m_offset;	
	boost::iostreams::file_sink m_stream;
	
	static std::string get_next_logfile(const std::string& directory, const std::string name, std::size_t max_file_size, std::size_t* offset = 0);
};

} /* mlog */

#endif /* __FILE_LOGGER_HPP__ */
