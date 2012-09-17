


#include <mlog/mlog.hpp>
#include <mlog/file_logger.hpp>

int main()
{
	try
	{
		mlog::mlogger.reset(new mlog::file_logger_thread_safe("test.txt", ".", 1024 * 1024));
		for (int i = 0; i < 100000; i++) 
		{
			MLOG_TRACE("write a line to the log.");
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
