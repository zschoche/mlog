#define BOOST_TEST_MODULE test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

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
		MLOG_INFO("this is a test."); 
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

BOOST_AUTO_TEST_CASE(mlog_log2_test)
{
	for(unsigned long i = 1; i < 1024*1024; i++)
		BOOST_CHECK_EQUAL(static_cast<unsigned long>(std::log2(i)), mlog::log2(i));
}

BOOST_AUTO_TEST_CASE(mlog_memory_logger_1_test)
{
	auto* log = new mlog::memory_logger<2>();
	mlog::mlogger.reset(log);
	MLOG_INFO("1");
	MLOG_INFO("2");
	BOOST_CHECK_EQUAL((*log)[0].text, "1");
	BOOST_CHECK_EQUAL((*log)[1].text, "2");
	BOOST_CHECK_EQUAL((*log)[2].text, "1"); //overflow
}


BOOST_AUTO_TEST_CASE(standard_logger_speed_test)
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

BOOST_AUTO_TEST_CASE(memory_logger_speed_test)
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

BOOST_AUTO_TEST_CASE(file_logger_speed_test)
{	
	mlog::mlogger.reset(new mlog::file_logger("log.txt"));
	mlog::mlogger->use_time(false);
	double  st_result = single_thread_test();
	mlog::mlogger->use_thread_id(true);
	double st_result_thread_id = single_thread_test();
	mlog::mlogger->use_time(true);
	double st_result_thread_id_time = single_thread_test();
		
	mlog::mlogger.reset(new mlog::file_logger_thread_safe("log.txt"));
	mlog::mlogger->use_time(false);
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

BOOST_AUTO_TEST_CASE(memory_logger_test)
{
	mlog::memory_logger<2048>* mem_log = new mlog::memory_logger<2048>();
	mlog::mlogger.reset(mem_log);
	mlog::mlogger->use_time(false);
	mlog::mlogger->use_thread_id(false);
	mlog::mlogger->use_time(false);

	for(std::size_t i = 0;i < 2048; i++)
	{
		MLOG_INFO(boost::format("%1%") % i);
	}
	
	for(std::size_t i = 0;i < 2048; i++)
	{
		BOOST_CHECK_EQUAL(boost::lexical_cast<std::size_t>((*mem_log)[i].text), i); 
	}

	//std::cout << *mem_log << std::endl;

}

