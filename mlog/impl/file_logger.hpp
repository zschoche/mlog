
#ifndef __FILE_LOGGER_IPP__
#define __FILE_LOGGER_IPP__

#include "../file_logger.hpp"
#include <sstream>
#include <string>

namespace mlog {

file_logger::file_logger(std::string log_name, std::string log_directory,
			 mlog_bytes max_file_size)
    : m_stream(get_next_logfile(log_directory, log_name, max_file_size,
				&m_offset),
	       BOOST_IOS::app),
      m_log_name(std::move(log_name)),
      m_log_directory(std::move(log_directory)),
      m_max_file_size(std::move(max_file_size)), m_offset(0) {}

file_logger::~file_logger() {}




void file_logger::flush() { m_stream.flush(); }

std::string file_logger::get_next_logfile(const std::string &directory,
					  const std::string name,
					  mlog_bytes max_file_size,
					  mlog_bytes *offset) {
	boost::filesystem::path path(directory);
	path /= name;
	std::string result;
	if (boost::filesystem::exists(path) &&
	    (max_file_size == 0 ||
	     boost::filesystem::file_size(path) > max_file_size)) {

		int i(0);
		boost::shared_ptr<boost::filesystem::path> new_path;
		bool exists(false);
		do {
			new_path.reset(new boost::filesystem::path(
			    path.string() + "." +
			    std::to_string(i)));
			i++;
			exists = boost::filesystem::exists(*new_path);
		} while (exists && (max_file_size == 0 ||
				    boost::filesystem::file_size(*new_path) >
					max_file_size));
		result = new_path->string();
	} else {
		result = path.string();
	}

	if (offset != 0) {
		if (boost::filesystem::exists(result))
			*offset = boost::filesystem::file_size(result);
		else
			*offset = 0;
	}
	return result;
}

} /* mlog */

#endif
