


#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>
int main()
{
	mlog::mlogger.reset(new mlog::file_logger("log.txt", ".", 1024 * 1024 * 5));
	MLOG_TRACE("write a trace line to your log.");
	MLOG_DEBUG("write a debug line to your log.");
	MLOG_INFO("write a info line to your log.");
	MLOG_ERROR("write a error line to your log.");
	MLOG_FATAL("write a fatal line to your log.");
	return 0;
}
