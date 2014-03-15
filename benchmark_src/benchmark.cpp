#include <mlog/mlog.hpp>
#include <mlog/memory_logger.hpp>
#include <mlog/file_logger.hpp>
#include <mlog/syslog_logger.hpp>
#include <mlog/async_logger.hpp>
#include <thread>
#include <mlog/multiple_loggers.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cmath>
#include <boost/generator_iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/file.hpp>

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

void write_some_log_entrys_boost() {
	for (int i = 0; i < num_loops; i++) {
		BOOST_LOG_TRIVIAL(info) << "this is a test.";
	}
}
double single_thread_test_boost() {
	boost::posix_time::ptime start =
	    boost::posix_time::microsec_clock::universal_time();
	write_some_log_entrys_boost();
	boost::posix_time::time_duration time =
	    boost::posix_time::microsec_clock::universal_time() - start;
	double ms = time.total_milliseconds() /
		    static_cast<double>(num_loops * num_threads);
	return ms;
}

struct empty_logger : mlog::logger<empty_logger> {
	template <typename M, typename T>
	void write_to_log(M &&metadata, T &&log_text) {
		metadata.to_string(log_text);
	}
};

struct result {
	double normal;
	double thread_safe;
	double async;
	double with_thread_id;
	double with_time;
	double with_pos;
};

result cout_logger() {
	result r;
	mlog::standard_logger log;
	mlog::manager->set_log(log);
	mlog::manager->set_default_settings();
	r.normal = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_thread_id(true);
	r.with_thread_id = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_time(true);
	r.with_time = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_position(true);
	r.with_pos = single_thread_test();

	mlog::standard_logger_thread_safe log_ts;
	mlog::manager->set_log(log_ts);
	mlog::manager->set_default_settings();
	r.thread_safe = single_thread_test();
	mlog::async_logger<mlog::standard_logger> log_async;
	mlog::manager->set_log(log_async);
	mlog::manager->set_default_settings();
	r.async = single_thread_test();
	log_async.flush();
	return r;
}

result file_logger() {
	result r;
	const std::string filename = "file_logger.log";
	std::remove(filename.c_str());
	mlog::file_logger log(filename);
	mlog::manager->set_log(log);
	mlog::manager->set_default_settings();
	r.normal = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_thread_id(true);
	r.with_thread_id = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_time(true);
	r.with_time = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_position(true);
	r.with_pos = single_thread_test();

	std::remove(filename.c_str());
	mlog::file_logger_thread_safe log_ts(filename);
	mlog::manager->set_log(log_ts);
	mlog::manager->set_default_settings();
	r.thread_safe = single_thread_test();
	std::remove(filename.c_str());
	mlog::async_logger<mlog::file_logger> log_async(filename);
	mlog::manager->set_log(log_async);
	mlog::manager->set_default_settings();
	r.async = single_thread_test();
	log_async.flush();
	std::cout << "file tests:" << std::endl;
	std::cout << "\t"
		  << "mlog::file_logger => " << r.normal << "ms" << std::endl;
	std::cout << "\t"
		  << "mlog::thread_safe<mlog::file_logger> => " << r.thread_safe
		  << "ms" << std::endl;
	std::cout << "\t"
		  << "mlog::async_logger<mlog::file_logger> => " << r.async
		  << "ms" << std::endl;
	std::cout << "\t\twith thread id:\t\t"
		  << ((r.with_thread_id - r.normal) > 0 ? "+" : "")
		  << r.with_thread_id - r.normal << "ms" << std::endl;
	std::cout << "\t\twith time:\t\t"
		  << ((r.with_time - r.normal) > 0 ? "+" : "")
		  << r.with_time - r.normal << "ms" << std::endl;
	std::cout << "\t\twith code position:\t"
		  << ((r.with_pos - r.normal) > 0 ? "+" : "")
		  << r.with_pos - r.normal << "ms" << std::endl;
	std::remove(filename.c_str());
	return r;
}

result syslog_logger() {
	result r;
	mlog::syslog_logger log;
	mlog::manager->set_log(log);
	mlog::manager->set_default_settings();
	r.normal = single_thread_test();
	mlog::manager->use_thread_id(true);
	r.with_thread_id = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_time(true);
	r.with_time = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_position(true);
	r.with_pos = single_thread_test();

	mlog::thread_safe<mlog::syslog_logger> log_ts;
	mlog::manager->set_log(log_ts);
	mlog::manager->set_default_settings();
	r.thread_safe = single_thread_test();
	mlog::async_logger<mlog::syslog_logger> log_async;
	mlog::manager->set_log(log_async);
	mlog::manager->set_default_settings();
	r.async = single_thread_test();
	log_async.flush();
	std::cout << "syslog tests:" << std::endl;
	std::cout << "\t"
		  << "mlog::syslog_logger => " << r.normal << "ms" << std::endl;
	std::cout << "\t"
		  << "mlog::thread_safe<mlog::syslog_logger> => "
		  << r.thread_safe << "ms" << std::endl;
	std::cout << "\t"
		  << "mlog::async_logger<mlog::syslog_logger> => " << r.async
		  << "ms" << std::endl;
	std::cout << "\t\twith thread id:\t\t"
		  << ((r.with_thread_id - r.normal) > 0 ? "+" : "")
		  << r.with_thread_id - r.normal << "ms" << std::endl;
	std::cout << "\t\twith time:\t\t"
		  << ((r.with_time - r.normal) > 0 ? "+" : "")
		  << r.with_time - r.normal << "ms" << std::endl;
	std::cout << "\t\twith code position:\t"
		  << ((r.with_pos - r.normal) > 0 ? "+" : "")
		  << r.with_pos - r.normal << "ms" << std::endl;

	return r;
}

result memory_logger() {
	result r;
	mlog::memory_logger_normal log;
	mlog::manager->set_log(log);
	mlog::manager->set_default_settings();
	r.normal = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_thread_id(true);
	r.with_thread_id = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_time(true);
	r.with_time = single_thread_test();
	mlog::manager->set_default_settings();
	mlog::manager->use_position(true);
	mlog::manager->set_default_settings();
	r.with_pos = single_thread_test();

	std::cout << "memory tests:" << std::endl;
	std::cout << "\t"
		  << "mlog::memory_logger => " << r.normal << "ms" << std::endl;
	std::cout << "\t\twith thread id:\t\t"
		  << ((r.with_thread_id - r.normal) > 0 ? "+" : "")
		  << r.with_thread_id - r.normal << "ms" << std::endl;
	std::cout << "\t\twith time:\t\t"
		  << ((r.with_time - r.normal) > 0 ? "+" : "")
		  << r.with_time - r.normal << "ms" << std::endl;
	std::cout << "\t\twith code position:\t"
		  << ((r.with_pos - r.normal) > 0 ? "+" : "")
		  << r.with_pos - r.normal << "ms" << std::endl;

	return r;
}

void frontend() {
	std::cout << "frontend tests:" << std::endl;
	empty_logger onlyfrontend;
	mlog::manager->set_log(onlyfrontend);
	mlog::manager->set_default_settings();
	std::cout << "\tdefault settings =>\t" << single_thread_test() << "ms"
		  << std::endl;
	mlog::manager->use_thread_id(true);
	std::cout << "\tadded thread id =>\t" << single_thread_test() << "ms"
		  << std::endl;
	mlog::manager->use_time(true);
	std::cout << "\tadded time =>\t\t" << single_thread_test() << "ms"
		  << std::endl;
	mlog::manager->use_position(true);
	std::cout << "\tadded position time =>\t" << single_thread_test()
		  << "ms" << std::endl;
}

void init() {
	boost::log::add_file_log("boost.log");

	boost::log::core::get()->set_filter(boost::log::trivial::severity >=
					    boost::log::trivial::info);
}

void compare_with_boost() {
	mlog::standard_logger_thread_safe log_ts;
	mlog::manager->set_log(log_ts);
	mlog::manager->use_thread_id(true);
	mlog::manager->use_time(true);
	double m = single_thread_test();
	m = single_thread_test();
	double b = single_thread_test_boost();
	b = single_thread_test_boost();

	
	std::cout << "### mlog benchmark ###" << std::endl << std::endl;

	std::cout << "compare with boost.log on stdout :" << std::endl;
	std::cout << "\tboost.log:\t" << b << "ms" << std::endl;
	std::cout << "\tmlog:\t\t" << m << "ms" << std::endl;

	std::remove("boost.log");
	std::remove("mlog.log");

	init();

	mlog::file_logger_thread_safe logfile("mlog.log");
	mlog::manager->set_log(logfile);
	b = single_thread_test_boost();
	b = single_thread_test_boost();
	m = single_thread_test();
	m = single_thread_test();
	std::remove("boost.log");
	std::remove("mlog.log");

	std::cout << "compare with boost.log on tests:" << std::endl;
	std::cout << "\tboost.log:\t" << b << "ms" << std::endl;
	std::cout << "\tmlog:\t\t" << m << "ms" << std::endl;
}

int main() {

	auto r = cout_logger();
	compare_with_boost();
	frontend();
	std::cout << "cout tests:" << std::endl;
	std::cout << "\t"
		  << "mlog::standard_logger => " << r.normal << "ms"
		  << std::endl;
	std::cout << "\t"
		  << "mlog::thread_safe<mlog::standard_logger> => "
		  << r.thread_safe << "ms" << std::endl;
	std::cout << "\t"
		  << "mlog::async_logger<mlog::standard_logger> => " << r.async
		  << "ms" << std::endl;
	std::cout << "\t\twith thread id:\t\t"
		  << ((r.with_thread_id - r.normal) > 0 ? "+" : "")
		  << r.with_thread_id - r.normal << "ms" << std::endl;
	std::cout << "\t\twith time:\t\t"
		  << ((r.with_time - r.normal) > 0 ? "+" : "")
		  << r.with_time - r.normal << "ms" << std::endl;
	std::cout << "\t\twith code position:\t"
		  << ((r.with_pos - r.normal) > 0 ? "+" : "")
		  << r.with_pos - r.normal << "ms" << std::endl;
	file_logger();
	syslog_logger();
	memory_logger();
	return 0;
}
