
#include <mlog/mlog.hpp>
#include <mlog/memory_logger.hpp>
#include <mlog/file_logger.hpp>
#include <thread>
#include <boost/date_time/posix_time/posix_time.hpp>

static const int num_threads = 2;
static const int num_loops = 100000;

void write_some_log_entrys()
{
	for(int i = 0; i < num_loops; i++)
		MLOG_INFO("this is a test.") 
}

double single_thread_test()
{
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time(); 
	write_some_log_entrys();
	boost::posix_time::time_duration time = boost::posix_time::microsec_clock::universal_time() - start;
	double ms = time.total_milliseconds() / static_cast<double>(num_loops * num_threads);
	return ms;
}

double multi_thread_test()
{
	std::thread threads[num_threads];
	
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	for (int i = 0; i < num_threads; ++i)
            	threads[i] = std::thread([]() 
		{
			write_some_log_entrys();
		});

	for(auto& t : threads)
		t.join();
	boost::posix_time::time_duration time = boost::posix_time::microsec_clock::universal_time() - start;
	double ms = time.total_milliseconds() / static_cast<double>(num_loops * num_threads);
	return ms; 
}

void standard_logger_test()
{
	mlog::mlogger.reset(new mlog::standard_logger());
	double  st_result = single_thread_test();
	mlog::mlogger->use_thread_id(true);
	double st_result_thread_id = single_thread_test();
	mlog::mlogger->use_time(true);
	double st_result_thread_id_time = single_thread_test();
		
	mlog::mlogger.reset(new mlog::standard_logger_thread_safe());
	double mt_result = multi_thread_test();
	mlog::mlogger->use_thread_id(true);
	double mt_result_thread_id = multi_thread_test();
	mlog::mlogger->use_time(true);
	double mt_result_thread_id_time = multi_thread_test();

	std::cout << std::endl;	
	std::cout << "### single-threaded standard logger test ###" << std::endl;
	std::cout << st_result << "ms for each log statment." << std::endl;
	std::cout << st_result_thread_id << "ms for each log statment with thread id." << std::endl;
	std::cout << st_result_thread_id_time << "ms for each log statment with thread id and timestamp." << std::endl;
	std::cout << std::endl;
	std::cout << "### multi-threaded standard logger test ###" << std::endl;
	std::cout << mt_result << "ms for each log statment." << std::endl;
	std::cout << mt_result_thread_id << "ms for each log statment with thread id." << std::endl;
	std::cout << mt_result_thread_id_time << "ms for each log statment with thread id and timestamp." << std::endl;
}

void memory_logger_test()
{
	mlog::mlogger.reset(new mlog::memory_logger_normal());
	double  st_result = single_thread_test();
	mlog::mlogger->use_thread_id(true);
	double st_result_thread_id = single_thread_test();
	mlog::mlogger->use_time(true);
	double st_result_thread_id_time = single_thread_test();
		
	double mt_result = multi_thread_test();
	mlog::mlogger->use_thread_id(true);
	double mt_result_thread_id = multi_thread_test();
	mlog::mlogger->use_time(true);
	double mt_result_thread_id_time = multi_thread_test();

	std::cout << std::endl;	
	std::cout << "### single-threaded memory logger test ###" << std::endl;
	std::cout << st_result << "ms for each log statment." << std::endl;
	std::cout << st_result_thread_id << "ms for each log statment with thread id." << std::endl;
	std::cout << st_result_thread_id_time << "ms for each log statment with thread id and timestamp." << std::endl;
	std::cout << std::endl;
	std::cout << "### multi-threaded memory logger test ###" << std::endl;
	std::cout << mt_result << "ms for each log statment." << std::endl;
	std::cout << mt_result_thread_id << "ms for each log statment with thread id." << std::endl;
	std::cout << mt_result_thread_id_time << "ms for each log statment with thread id and timestamp." << std::endl;


}

void file_logger_test()
{	
	mlog::mlogger.reset(new mlog::file_logger("log.txt"));
	double  st_result = single_thread_test();
	mlog::mlogger->use_thread_id(true);
	double st_result_thread_id = single_thread_test();
	mlog::mlogger->use_time(true);
	double st_result_thread_id_time = single_thread_test();
		
	mlog::mlogger.reset(new mlog::file_logger_thread_safe("log.txt"));
	double mt_result = multi_thread_test();
	mlog::mlogger->use_thread_id(true);
	double mt_result_thread_id = multi_thread_test();
	mlog::mlogger->use_time(true);
	double mt_result_thread_id_time = multi_thread_test();

	std::cout << std::endl;	
	std::cout << "### single-threaded file logger test ###" << std::endl;
	std::cout << st_result << "ms for each log statment." << std::endl;
	std::cout << st_result_thread_id << "ms for each log statment with thread id." << std::endl;
	std::cout << st_result_thread_id_time << "ms for each log statment with thread id and timestamp." << std::endl;
	std::cout << std::endl;
	std::cout << "### multi-threaded file logger test ###" << std::endl;
	std::cout << mt_result << "ms for each log statment." << std::endl;
	std::cout << mt_result_thread_id << "ms for each log statment with thread id." << std::endl;
	std::cout << mt_result_thread_id_time << "ms for each log statment with thread id and timestamp." << std::endl;




}


int main()
{
	standard_logger_test();	
	memory_logger_test();
	file_logger_test();
	return 0;
}
