#define BOOST_TEST_MODULE test

#include <boost/test/unit_test.hpp>

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
#include <boost/regex.hpp>

#ifdef _MSC_VER
namespace std {
double log2(double n) {
	// log(n)/log(2) is log2.
	return log(n) / log(2);
}
}
#endif

struct empty_logger : mlog::logger<empty_logger> {

	template <typename M, typename T>
	void write_to_log(M &&metadata, T &&log_text) {
		 std::string str = metadata.to_string(std::forward<T>(log_text), false);
		 str = metadata.to_string(std::forward<T>(log_text), true);
	}
};

BOOST_AUTO_TEST_CASE(mlog_empty_test) {}


BOOST_AUTO_TEST_CASE(mlog_log2_test) {
	for (unsigned long i = 1; i < 1024 * 1024; i++)
		BOOST_CHECK_EQUAL(static_cast<unsigned long>(std::log2(i)),
				  mlog::log2(i));
	std::cout << "mlog_log2_test passed." << std::endl;
}


BOOST_AUTO_TEST_CASE(mlog_metadata_overdose_test) {
	std::string file(256,'a');
	std::string text(1024,'t');
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename("test.cpp"), "test.cpp");
#if defined(_WIN32_WCE) || defined(_WIN16) || defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename("c:\\temp\\test.cpp"), "test.cpp");
#else
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename("/tmp/test.cpp"), "test.cpp");
#endif
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename(""), "");
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename("/"), "/");
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename(std::string(256,'a').c_str()), std::string(256,'a').c_str());
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename(std::string(257,'a').c_str()), std::string(256,'a').c_str());
	BOOST_CHECK_EQUAL(mlog::log_position::cut_filename(std::string(512,'a').c_str()), std::string(256,'a').c_str());
}

BOOST_AUTO_TEST_CASE(mlog_memory_logger_1_test) {
	mlog::memory_logger<2> log;
	mlog::manager->set_log(&log);
	MLOG_INFO("1");
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		log.to_string(),
		boost::regex("^\\[[0-9][0-9]\\]\\{info\\}: 1\\n")),
	    true);
	MLOG_INFO("2");
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		log.to_string(),
		boost::regex("^\\[[0-9][0-9]\\]\\{info\\}: "
			     "1\\n\\[[0-9][0-9]\\]\\{info\\}: 2\n")),
	    true);
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
	MLOG_INFO("this goes to the cout.");
	std::cout << "mlog_logger_cleanup_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(mlog_memory_logger_mt_test) {

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

BOOST_AUTO_TEST_CASE(mlog_async_test) {

	mlog::async_logger<mlog::memory_logger_normal> log;
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
	log.flush();

	int result[512];
	for (auto &&i : result) {
		i = 0;
	}

	for (int i = 0; i < log.get().size(); i++) {
		std::string s = log.get()[i].text;
		int n = std::atoi(s.c_str());
		result[n]++;
	}

	for (auto &&i : result) {
		BOOST_CHECK_EQUAL(i, 8);
	}
	std::cout << "mlog_async_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(mlog_marco_test) {
	const std::string filename = "macro_test.log";
	std::remove(filename.c_str());
	mlog::file_logger_thread_safe log(filename);
	mlog::manager->set_log(log);

	MLOG_INFO("info");
	MLOG_DEBUG("debug");
	MLOG_ERROR("error");
	MLOG_FATAL("fatal");
	MLOG_TRACE("trace");
	MLOG_WARNING("warning");
	log.get().flush();

	std::ifstream file(filename, std::ios_base::in | std::ios_base::binary);
	BOOST_CHECK_EQUAL(file.is_open(), true);
	std::string str;
	std::getline(file, str);
	std::cout << str << std::endl;
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^\\[[0-9][0-9]\\]\\{info\\}: info$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^\\[[0-9][0-9]\\]\\{debug\\}: debug$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^\\[[0-9][0-9]\\]\\{error\\}: error$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^\\[[0-9][0-9]\\]\\{fatal\\}: fatal$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^\\[[0-9][0-9]\\]\\{trace\\}: trace$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^\\[[0-9][0-9]\\]\\{warning\\}: warning$")),
	    true);
	std::remove(filename.c_str());
	std::cout << "mlog_marco_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(mlog_options_test) {
	const std::string filename = "options_test.log";
	std::remove(filename.c_str());
	mlog::file_logger_thread_safe log(filename);
	mlog::manager->set_log(log);
	MLOG_INFO("none");
	mlog::manager->use_thread_id(true);
	MLOG_INFO("thread_id");
	mlog::manager->use_thread_id(false);
	mlog::manager->use_time(true);
	MLOG_INFO("time");
	mlog::manager->use_time(false);
	mlog::manager->use_position(true);
	MLOG_INFO("position");
	mlog::manager->use_thread_id(true);
	mlog::manager->use_time(true);
	MLOG_INFO("all");
	mlog::manager->set_default_settings();
	log.get().flush();
	std::ifstream file(filename, std::ios_base::in | std::ios_base::binary);
	BOOST_CHECK_EQUAL(file.is_open(), true);
	std::string str;
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^\\[[0-9][0-9]\\]\\{info\\}: none$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str,
		boost::regex("^\\[[0-9][0-9]-.*\\]\\{info\\}: thread_id$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex("^.*\\[[0-9][0-9]\\]\\{info\\}: time$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex(
			 "^\\[[^:]*:[0-9]* [0-9]*\\]\\{info\\}: position$")),
	    true);
	std::getline(file, str);
	BOOST_CHECK_EQUAL(
	    boost::regex_match(
		str, boost::regex(
			 "^.*\\[[^:]*:[0-9]* [0-9]*-.*\\]\\{info\\}: all$")),
	    true);
	std::remove(filename.c_str());
	std::cout << "mlog_marco_test passed." << std::endl;
}

boost::regex
stdline("^\\[[0-9].\\]\\{(info|debung|trace|warning|error|fatal)\\}: [0-9]*$");

BOOST_AUTO_TEST_CASE(mlog_file_logger_test) {

	const std::string filename = "file_logger_test.log";
	std::remove(filename.c_str());
	mlog::file_logger_thread_safe log(filename);
	mlog::manager->set_log(log);
	for (int i = 0; i < 512; i++) {
		MLOG_INFO(boost::lexical_cast<std::string>(i));
	}

	log.get().flush();
	std::ifstream file(filename, std::ios_base::in | std::ios_base::binary);
	BOOST_CHECK_EQUAL(file.is_open(), true);

	int i = 0;
	for (std::string str; std::getline(file, str); i++) {
		int index = str.find_last_of(" ");
		BOOST_CHECK(index != std::string::npos);
		std::string num = str.substr(index + 1);
		BOOST_CHECK_EQUAL(std::atoi(num.c_str()), i);
		BOOST_CHECK_EQUAL(boost::regex_match(str, stdline), true);
	}
	std::remove(filename.c_str());
	std::cout << "mlog_file_logger_test passed." << std::endl;
}

BOOST_AUTO_TEST_CASE(mlog_file_logger_mt_test) {

	const std::string filename = "file_logger_test.log";
	std::remove(filename.c_str());
	mlog::file_logger_thread_safe log(filename);
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

	log.get().flush();

	int result[512];
	for (auto &&i : result) {
		i = 0;
	}

	std::ifstream file(filename, std::ios_base::in | std::ios_base::binary);
	BOOST_CHECK_EQUAL(file.is_open(), true);

	for (std::string str; std::getline(file, str);) {
		int index = str.find_last_of(" ");
		BOOST_CHECK(index != std::string::npos);
		std::string num = str.substr(index + 1);
		int n = std::atoi(num.c_str());
		result[n]++;
		BOOST_CHECK_EQUAL(boost::regex_match(str, stdline), true);
	}

	for (auto &&i : result) {
		BOOST_CHECK_EQUAL(i, 8);
	}
	std::cout << "mlog_file_logger_mt_test passed." << std::endl;
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

BOOST_AUTO_TEST_CASE(memory_logger_test) {
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



BOOST_AUTO_TEST_CASE(mlog_heavy_test) {

	empty_logger log;
	mlog::manager->set_log(log);
	std::string str = "heavy";
	for (int i = 0; i < 16; i++) {
		str += str;
		MLOG_INFO(str);
	}
	std::cout << "mlog_heavy_test passed." << std::endl;
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

		int id = mlog::pseudo_random_number(1024 * 1024);

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
