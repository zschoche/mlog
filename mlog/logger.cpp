

#include "logger.hpp"
#include <sstream>

namespace mlog 
{
	

logger::logger()
{
	boost::random::mt19937 rng(std::time(0));
	boost::random::uniform_int_distribution<> six(0, 100);
	m_session = six(rng);  
}


logger::~logger()
{
}

void logger::write(const std::string& log_text)
{
	std::stringstream ss;
	ss << boost::posix_time::microsec_clock::local_time() << "[" << m_session << "-" << boost::this_thread::get_id() << "]-" << log_text;  
	write_to_log(ss.str());
}

void logger::write_to_log(const std::string& log_text)
{
	std::cout << log_text << std::endl;
}

void logger::flush()
{
	std::cout.flush();
}

} /* mlog */
