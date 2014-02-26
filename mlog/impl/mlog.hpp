/*
 * mlog.cpp
 *
 *  Created on: Aug 12, 2012
 *  Author: philipp
 */

#ifndef __MLOG_IPP__
#define __MLOG_IPP__

#include "../mlog.hpp"
#include <boost/random.hpp>

namespace mlog {

// This should not be cleaned up https://github.com/zschoche/mlog/issues/11
mlog_manager *manager = new mlog_manager();

mlog_manager::mlog_manager()
    : m_log(new standard_logger()), m_is_valid(false), m_use_time(false),
      m_use_thread_id(false), m_use_position(false) {
	using namespace boost;
	using namespace boost::random;
	mt19937 rng(std::time(0));

#ifdef BOOST_HAS_TR1_RANDOM
	uniform_int_distribution<> six(0, 100);
	m_session = six(rng);
#elseif
	boost::uniform_int<> six(0, 100);
	boost::variate_generator<boost::mt19937 &, boost::uniform_int<> > die(
	    rng, six);
	m_session = die();
#endif
}

void mlog_manager::set_log(logger_base *log) {
	logger_base *old_log = m_log;

	if (log == nullptr) {
		m_log = new standard_logger();
	} else {
		m_log = log;
	}

	if (!m_is_valid)
		delete old_log;
	m_is_valid = log != nullptr;
}
}

#endif
