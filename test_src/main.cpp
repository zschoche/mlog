


#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>
	
int main()
{
	mlog::mlogger.reset(new mlog::thread_safe<mlog::file_logger>("log.txt", ".", 1024 * 1024 * 5));
	MLOG_TRACE("this is thread safe.");
	return 0;
}
