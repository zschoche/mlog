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
	mlog_manager* manager = new mlog_manager();
}

#endif
