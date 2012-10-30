
#include <mlog/mlog.hpp>
#include <mlog/memory_logger.hpp>


int main()
{
	try
	{
		mlog::memory_logger_small* log = new mlog::memory_logger_small(); 
		mlog::mlogger.reset(log);

		for(int i = 0; i < 130; i++)
		{
			MLOG_TRACE("entry nr." << i);
		}

		std::cout << *log; 
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
