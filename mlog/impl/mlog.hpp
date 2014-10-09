/*
 * mlog.cpp
 *
 *  Created on: Aug 12, 2012
 *  Author: philipp
 */

#ifndef __MLOG_IPP__
#define __MLOG_IPP__

#include "../mlog.hpp"

namespace mlog {

// This should not be cleaned up https://github.com/zschoche/mlog/issues/11
mlog_manager *manager = new mlog_manager();

mlog_manager::mlog_manager()
    : m_log(new standard_logger()), m_is_valid(false),
      m_session(mlog::pseudo_random_number()), m_use_time(false),
      m_use_thread_id(false), m_use_position(false) {}

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

unsigned int pseudo_random_number(unsigned int max) {
	srand(time(0));
	unsigned int result = rand() % max;
	return result;
}
}

#endif
