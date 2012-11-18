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
#include "thread_safe.hpp"

typedef std::size_t mlog_bytes;

/*
constexpr mlog_bytes operator"" _KB(int kb)
{
	return kb * 1024; 
}

constexpr mlog_bytes operator"" _MB(std::size_t mb)
{
	return mb * 1024 * 1024;
}

constexpr mlog_bytes operator"" _GB(std::size_t bg)
{
	return gb * 1024 * 1024 * 1024;
}

*/

namespace mlog 
{


class file_logger : public logger  
{
public:
	
	file_logger(std::string log_name, std::string log_directory = ".", mlog_bytes max_file_size = 5 * 1024 * 1024 /*5_MB*/);
	virtual ~file_logger();
	virtual void write_to_log(log_metadata&& metadata, std::string&& log_text); 

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
	mlog_bytes m_max_file_size;
	mlog_bytes m_offset;	
	boost::iostreams::file_sink m_stream;
	
	static std::string get_next_logfile(const std::string& directory, const std::string name, mlog_bytes max_file_size, mlog_bytes* offset = 0);
};

typedef thread_safe<file_logger> file_logger_thread_safe; 

} /* mlog */

#endif /* __FILE_LOGGER_HPP__ */

#ifdef MLOG_NO_LIB
#include "impl/file_logger.ipp"
#endif

