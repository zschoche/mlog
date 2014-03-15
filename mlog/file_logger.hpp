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

namespace mlog {

class file_logger : public logger<file_logger> {
      public:

	file_logger(std::string log_name, std::string log_directory = ".",
		    mlog_bytes max_file_size = 5 * 1024 * 1024 /*5_MB*/);
	virtual ~file_logger();

	template <typename M, typename T>
	void write_to_log(M &&metadata, T &&log_text) {
		std::string str = metadata.to_string(std::forward<T>(log_text), true);
		m_stream.write(str.c_str(), str.size());
		m_offset += str.size(); // + 1;

		if(m_flush_immediately)
			flush();

		if (max_file_size() != 0 && m_offset > max_file_size()) {
			m_stream.open(
			    get_next_logfile(m_log_directory, m_log_name,
					     max_file_size(), &m_offset), std::ios::binary);
		}
	}

	template <typename T> inline void max_file_size(T value) {
		m_max_file_size = std::forward<T>(value);
	}

	inline std::size_t max_file_size() const { return m_max_file_size; }

	inline bool is_open() const { return m_stream.is_open(); }

	void flush();

	inline bool flush_immediately() const {
		return m_flush_immediately;
	}

	template<typename T>
	void flush_immediately(T&& value) {
		m_flush_immediately = value;
	}
	



      private:

	bool m_flush_immediately;

	std::ofstream m_stream;
	std::string m_log_name;
	std::string m_log_directory;
	mlog_bytes m_max_file_size;
	mlog_bytes m_offset;

	static std::string get_next_logfile(const std::string &directory,
					    const std::string name,
					    mlog_bytes max_file_size,
					    mlog_bytes *offset = 0);
};

typedef thread_safe<file_logger> file_logger_thread_safe;

} /* mlog */

#endif /* __FILE_LOGGER_HPP__ */

#ifdef MLOG_NO_LIB
#include "impl/file_logger.hpp"
#endif
