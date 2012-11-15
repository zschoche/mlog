/*
 * log.cpp
 *
 *  Created on: Aug 12, 2012
 *      Author: philipp
 */


#include "mlog.hpp"

namespace mlog
{

std::unique_ptr<logger> mlogger(new standard_logger());



}
