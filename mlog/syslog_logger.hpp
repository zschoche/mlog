/*
*
*	Author: Philipp Zschoche
*	GitHub: https://github.com/zschoche
*
*/
#if !defined(_WIN32) && (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)))
#ifndef __SYSLOG_LOGGERS_HPP__
#define __SYSLOG_LOGGERS_HPP__

#include <syslog.h>
#include "logger.hpp"
		
namespace   mlog 
{
	class syslog_logger : public logger 
	{
	public:
		enum syslog_level {
			EMERG =		LOG_EMERG,	/* system is unusable */
			ALERT =		LOG_ALERT,	/* action must be taken immediately */
			CRIT =		LOG_CRIT,	/* critical conditions */
			ERR	=	LOG_ERR,	/* error conditions */
			WARNING =	LOG_WARNING,	/* warning conditions */
			NOTICE =	LOG_NOTICE,	/* normal but significant condition */
			INFO =		LOG_INFO,	/* informational */
			DEBUG = 	LOG_DEBUG	/* debug */		
		};
	
		  syslog_logger(const char* ident = nullptr, int option = LOG_CONS | LOG_PID | LOG_NDELAY, int facility = LOG_LOCAL1)
		  :level(syslog_level::NOTICE) {
     			setlogmask(LOG_UPTO(syslog_level::DEBUG)); // mlog handle that by its own.
			openlog(ident, option, facility);
		}

		virtual ~syslog_logger() {
			closelog();
		}

		void write_to_log(log_metadata &&metadata,
				  std::string &&log_text) {
			syslog(level, "%s", metadata.to_string(log_text).c_str());
		}

		void write_to_log(const log_metadata& metadata,
				  const std::string& log_text) {
			syslog(level, "%s", metadata.to_string(log_text).c_str());
		}
		
		syslog_level level;


	};
} /*   mlog */


#endif /* ___HPP__ */
#endif 

