


#include <mlog/mlog.hpp>

int main()
{
	MLOG_TRACE("write a trace line to your log.");
	MLOG_DEBUG("write a debug line to your log.");
	MLOG_INFO("write a info line to your log.");
	MLOG_ERROR("write a error line to your log.");
	MLOG_FATAL("write a fatal line to your log.");
	return 0;
}
