#define BOOST_TEST_MODULE test

#include <boost/test/unit_test.hpp>

#include <mlog/mlog.hpp>
#include <mlog/memory_logger.hpp>
#include <mlog/file_logger.hpp>
#include <mlog/syslog_logger.hpp>
#include <thread>
#include <mlog/multiple_loggers.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cmath>
#include <boost/random.hpp>
#include <boost/generator_iterator.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _MSC_VER
namespace std {
double log2(double n) {
	// log(n)/log(2) is log2.
	return log(n) / log(2);
}
}
#endif

const int num_threads = 2;
int num_loops = 100000;

void write_some_log_entrys() {
	for (int i = 0; i < num_loops; i++)
		MLOG_INFO("this is a test.");
}

double single_thread_test() {
	boost::posix_time::ptime start =
	    boost::posix_time::microsec_clock::universal_time();
	write_some_log_entrys();
	boost::posix_time::time_duration time =
	    boost::posix_time::microsec_clock::universal_time() - start;
	double ms = time.total_milliseconds() /
		    static_cast<double>(num_loops * num_threads);
	return ms;
}

double multi_thread_test() {
	std::thread threads[num_threads];

	boost::posix_time::ptime start =
	    boost::posix_time::microsec_clock::universal_time();
	for (int i = 0; i < num_threads; ++i)
		threads[i] = std::thread([]() { write_some_log_entrys(); });

	for (auto &t : threads)
		t.join();
	boost::posix_time::time_duration time =
	    boost::posix_time::microsec_clock::universal_time() - start;
	double ms = time.total_milliseconds() /
		    static_cast<double>(num_loops * num_threads);
	return ms;
}

BOOST_AUTO_TEST_CASE(mlog_empty_test) {}

BOOST_AUTO_TEST_CASE(mlog_log2_test) {
	for (unsigned long i = 1; i < 1024 * 1024; i++)
		BOOST_CHECK_EQUAL(static_cast<unsigned long>(std::log2(i)),
				  mlog::log2(i));
	std::cout << "mlog_log2_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(mlog_memory_logger_1_test) {
	mlog::memory_logger<2> log;
	mlog::manager->set_log(&log);
	MLOG_INFO("1");
	MLOG_INFO("2");
	BOOST_CHECK_EQUAL(log[0].text, "1");
	BOOST_CHECK_EQUAL(log[1].text, "2");
	BOOST_CHECK_EQUAL(log[2].text, "1"); // overflow
	std::cout << "mlog_memory_logger_1_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(mlog_logger_cleanup_test) {

	auto *log = new mlog::memory_logger<2>();
	mlog::manager->set_log(log);
	MLOG_INFO("1");
	delete log;
	MLOG_INFO("this gos to the cout.");
	std::cout << "mlog_logger_cleanup_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(mlog_memory_logger_atomic_test) {

	mlog::memory_logger_normal log;
	mlog::manager->set_log(log);
	const std::size_t t = 8;
	std::thread threads[t];

	for (int i = 0; i < t; ++i)
		threads[i] = std::thread([&]() {
			for (int k = 0; k < 512; k++) { // (log size) / 8 = 512
				MLOG_INFO(boost::lexical_cast<std::string>(k));
			}
		});

	for (auto &t : threads)
		t.join();

	int result[512];
	for (auto &&i : result) {
		i = 0;
	}

	for (int i = 0; i < log.size(); i++) {
		std::string s = log[i].text;
		int n = std::atoi(s.c_str());
		result[n]++;
	}

	for (auto &&i : result) {
		BOOST_CHECK_EQUAL(i, 8);
	}
	std::cout << "mlog_memory_logger_atomic_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(multiple_loggers_test) {
	mlog::multiple_loggers log;
	std::unique_ptr<mlog::memory_logger<2> > log1(
	    new mlog::memory_logger<2>());
	std::unique_ptr<mlog::memory_logger<2> > log2(
	    new mlog::memory_logger<2>());
	std::unique_ptr<mlog::memory_logger<2> > log3(
	    new mlog::memory_logger<2>());
	log.m_loggers.push_back(log1.get());
	log.m_loggers.push_back(log2.get());
	log.m_loggers.push_back(log3.get());
	mlog::manager->set_log(log);
	MLOG_INFO("TEST1");
	MLOG_INFO("TEST2");
	BOOST_CHECK_EQUAL((*log1)[0].text, "TEST1");
	BOOST_CHECK_EQUAL((*log1)[1].text, "TEST2");
	BOOST_CHECK_EQUAL((*log2)[0].text, "TEST1");
	BOOST_CHECK_EQUAL((*log2)[1].text, "TEST2");
	BOOST_CHECK_EQUAL((*log3)[0].text, "TEST1");
	BOOST_CHECK_EQUAL((*log3)[1].text, "TEST2");
	std::cout << "multiple_loggers_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(standard_logger_speed_test) {
	num_loops = 100;
	mlog::standard_logger stdlog;
	mlog::manager->set_log(stdlog);
	double st_result = single_thread_test();
	mlog::manager->use_thread_id(true);
	double st_result_thread_id = single_thread_test();
	mlog::manager->use_time(true);
	double st_result_thread_id_time = single_thread_test();
	mlog::manager->use_position(true);
	double st_result_thread_id_time_pos = single_thread_test();

	mlog::standard_logger_thread_safe stdlog_thread_safe;
	mlog::manager->set_log(stdlog_thread_safe);
	double mt_result = single_thread_test();
	mlog::manager->use_thread_id(true);
	double mt_result_thread_id = single_thread_test();
	mlog::manager->use_time(true);
	double mt_result_thread_id_time = single_thread_test();
	mlog::manager->use_position(true);
	double mt_result_thread_id_time_pos = single_thread_test();

	std::cout << std::endl;
	std::cout << "### single-threaded standard logger test ###"
		  << std::endl;
	std::cout << st_result << "ms for each log statment." << std::endl;
	std::cout << st_result_thread_id
		  << "ms for each log statment with thread id." << std::endl;
	std::cout << st_result_thread_id_time
		  << "ms for each log statment with thread id and timestamp."
		  << std::endl;
	std::cout << st_result_thread_id_time_pos
		  << "ms for each log statment with thread id, timestamp and "
		     "position." << std::endl;
	std::cout << std::endl;
	std::cout << "### multi-threaded standard logger test ###" << std::endl;
	std::cout << mt_result << "ms for each log statment." << std::endl;
	std::cout << mt_result_thread_id
		  << "ms for each log statment with thread id." << std::endl;
	std::cout << mt_result_thread_id_time
		  << "ms for each log statment with thread id and timestamp."
		  << std::endl;
	std::cout << mt_result_thread_id_time_pos
		  << "ms for each log statment with thread id, timestamp and "
		     "position." << std::endl;
	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(memory_logger_speed_test) {
	num_loops = 100000;
	mlog::memory_logger_normal log;
	mlog::manager->set_log(log);
	double st_result = single_thread_test();
	mlog::manager->use_thread_id(true);
	double st_result_thread_id = single_thread_test();
	mlog::manager->use_time(true);
	double st_result_thread_id_time = single_thread_test();
	mlog::manager->use_position(true);
	double st_result_thread_id_time_pos = single_thread_test();

	std::cout << std::endl;
	std::cout << "### memory logger test ###" << std::endl;
	std::cout << st_result << "ms for each log statment." << std::endl;
	std::cout << st_result_thread_id
		  << "ms for each log statment with thread id." << std::endl;
	std::cout << st_result_thread_id_time
		  << "ms for each log statment with thread id and timestamp."
		  << std::endl;
	std::cout << st_result_thread_id_time_pos
		  << "ms for each log statment with thread id, timestamp and "
		     "position." << std::endl;
	std::cout << std::endl;
}

BOOST_AUTO_TEST_CASE(file_logger_speed_test) {
	num_loops = 100000;
	mlog::file_logger log("log.txt");
	mlog::manager->set_log(log);
	mlog::manager->use_time(false);
	double st_result = single_thread_test();
	mlog::manager->use_thread_id(true);
	double st_result_thread_id = single_thread_test();
	mlog::manager->use_time(true);
	double st_result_thread_id_time = single_thread_test();
	mlog::manager->use_position(true);
	double st_result_thread_id_time_pos = single_thread_test();

	mlog::file_logger_thread_safe log_thread_safe("log.txt");
	mlog::manager->set_log(log_thread_safe);
	mlog::manager->use_time(false);
	double mt_result = single_thread_test();
	mlog::manager->use_thread_id(true);
	double mt_result_thread_id = single_thread_test();
	mlog::manager->use_time(true);
	double mt_result_thread_id_time = single_thread_test();
	mlog::manager->use_position(true);
	double mt_result_thread_id_time_pos = single_thread_test();

	std::cout << std::endl;
	std::cout << "### single-threaded file logger test ###" << std::endl;
	std::cout << st_result << "ms for each log statment." << std::endl;
	std::cout << st_result_thread_id
		  << "ms for each log statment with thread id." << std::endl;
	std::cout << st_result_thread_id_time
		  << "ms for each log statment with thread id and timestamp."
		  << std::endl;
	std::cout << st_result_thread_id_time_pos
		  << "ms for each log statment with thread id, timestamp and "
		     "position." << std::endl;
	std::cout << std::endl;
	std::cout << "### multi-threaded file logger test ###" << std::endl;
	std::cout << mt_result << "ms for each log statment." << std::endl;
	std::cout << mt_result_thread_id
		  << "ms for each log statment with thread id." << std::endl;
	std::cout << mt_result_thread_id_time
		  << "ms for each log statment with thread id and timestamp."
		  << std::endl;
	std::cout << mt_result_thread_id_time_pos
		  << "ms for each log statment with thread id, timestamp and "
		     "position." << std::endl;
}

BOOST_AUTO_TEST_CASE(memory_logger_test) {
	num_loops = 100000;
	mlog::memory_logger<2048> log;
	mlog::manager->set_log(log);
	mlog::manager->use_time(false);
	mlog::manager->use_thread_id(false);
	mlog::manager->use_time(false);

	for (std::size_t i = 0; i < 2048; i++) {
		MLOG_INFO(boost::format("%1%") % i);
	}

	for (std::size_t i = 0; i < 2048; i++) {
		BOOST_CHECK_EQUAL(std::atoi(log[i].text.c_str()), i);
	}
	std::cout << "memory_logger_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(memory_logger_test_small) {
	num_loops = 100000;
	mlog::memory_logger<4> mem_log;
	mlog::manager->set_log(mem_log);
	mlog::manager->use_time(false);
	mlog::manager->use_thread_id(false);
	mlog::manager->use_time(false);

	for (std::size_t i = 0; i < 1024 * 1024; i++) {
		MLOG_INFO(boost::format("%1%") % i);
	}
	std::cout << "memory_logger_test_small passed." << std::endl;
}

void find_log_messages(std::string &&log, std::vector<std::string> &strs) {
	std::ifstream file(log, std::ios_base::in | std::ios_base::binary);
	if (file.is_open()) {
		for (std::string str; std::getline(file, str);) {
			for (int i = strs.size() - 1; i >= 0; i--) {
				if (boost::algorithm::contains(str, strs[i])) {
					strs.erase(strs.begin() + i);
				}
			}
		}
	}
}

#if !defined(TRAVIS) && !defined(_WIN32) &&                                    \
    (defined(__unix__) || defined(__unix) ||                                   \
     (defined(__APPLE__) && defined(__MACH__)))
BOOST_AUTO_TEST_CASE(mlog_syslog_test) {

	if (std::getenv("TRAVIS") == nullptr) {

		boost::mt19937 rng;
		rng.seed(static_cast<unsigned int>(std::time(0)));
		boost::variate_generator<boost::mt19937, boost::uniform_int<> >
		dice(rng, boost::uniform_int<>(1, 1024 * 1024));

		int id = dice();

		std::string debug = boost::str(
		    boost::format("This is a debug test (%1%)") % id);
		std::string trace = boost::str(
		    boost::format("This is a trace test (%1%)") % id);
		std::string info =
		    boost::str(boost::format("This is a info test (%1%)") % id);
		std::string error = boost::str(
		    boost::format("This is a error test (%1%)") % id);
		std::string warning = boost::str(
		    boost::format("This is a warning test (%1%)") % id);
		std::string fatal = boost::str(
		    boost::format("This is a fatal test (%1%)") % id);
		{
			mlog::syslog_logger log("mlog_syslog_test");
			mlog::manager->set_log(&log);

			MLOG_TRACE(trace);
			MLOG_DEBUG(debug);
			MLOG_INFO(info);
			MLOG_ERROR(error);
			MLOG_WARNING(warning);
			MLOG_FATAL(fatal);
		}

		std::vector<std::string> strs = { debug, trace,   info,
						  error, warning, fatal };

		find_log_messages("/var/log/system.log", strs);
		find_log_messages("/var/log/messages", strs);
		find_log_messages("/var/log/syslog", strs);
		BOOST_CHECK_EQUAL(strs.size(), 0);
		std::cout << "mlog_syslog_test passed." << std::endl;
	}
}
#endif

#ifdef _MSC_VER
BOOST_AUTO_TEST_CASE(memory_leak) {
	std::cout << "msvc will detect a memory leak at the end of this "
		     "program. - it's a false positive." << std::endl;
}
#endif
// */
