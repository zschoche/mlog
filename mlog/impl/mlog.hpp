/*
 * log.cpp
 *
 *  Created on: Aug 12, 2012
 *      Author: philipp
 */

#ifndef __MLOG_IPP__
#define __MLOG_IPP__

#include "../mlog.hpp"

namespace mlog
{

std::unique_ptr<logger> mlogger(new standard_logger());



}

#endif
