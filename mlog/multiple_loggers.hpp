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
	class multiple_loggers : public logger<multiple_loggers> 
	{
	public:
		template<typename M, typename T>	
		inline	void write_to_log(M&& metadata,
				  T&& log_text) {

			std::vector<std::future<void>> r(m_loggers.size());
			for(auto&& item : m_loggers) {
				r.push_back(std::move(std::async([&](logger_base* l) { l->write_to_log(std::forward<M>(metadata), std::forward<T>(log_text)); }, item)));
			}
			for(auto&& item : r) {
				if(item.valid()) {
					item.wait();
				}
			}
		}
		
		std::vector<logger_base*> m_loggers;
	
	};
	
	
} /*   mlog */


#endif /* ___HPP__ */
