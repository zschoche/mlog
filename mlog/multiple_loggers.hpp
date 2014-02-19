/*
*
*	Author: Philipp Zschoche
*	GitHub: https://github.com/zschoche
*
*/
#ifndef __MULTIPLE_LOGGERS_HPP__
#define __MULTIPLE_LOGGERS_HPP__

#include <vector>
#include <future>

namespace   mlog 
{
	class multiple_loggers : public logger 
	{
	public:
	
		  multiple_loggers() { }
		virtual ~multiple_loggers() { }
		void write_to_log(log_metadata &&metadata,
				  std::string &&log_text) {
			write_to_log(metadata, log_text);	

		}
		void write_to_log(const log_metadata& metadata,
				  const std::string& log_text) {

			std::vector<std::future<void>> r(m_loggers.size());
			for(logger* item : m_loggers) {
				r.push_back(std::move(std::async([&](logger* l) { l->write_to_log(metadata, log_text); }, item)));
			}
			for(auto&& item : r) {
				if(item.valid()) {
					item.wait();
				}
			}
		}
		
		std::vector<logger*> m_loggers;
	
	};
	
	
} /*   mlog */


#endif /* ___HPP__ */
