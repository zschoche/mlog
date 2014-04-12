/*
 * logger.hpp
 *
 *  Created on: Aug 9, 2012
 *      Author: philipp
 */

#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <boost/format.hpp>
#include <cstdlib>
#include "manager.hpp"
#include "metadata.hpp"

namespace mlog {

extern mlog_manager *manager;

struct logger_base {

	logger_base() {}

	virtual ~logger_base() {
		if (mlog::manager->log() == this)
			mlog::manager->unset_log();
	}

	inline void write(mlog_level &&level, boost::format &&format,
			  log_position &&pos) {
		const log_metadata metadata(
		    std::move(level), std::move(pos));
		write_to_log(std::move(metadata), boost::str(format));
	}

	inline void write(mlog_level &&level, const boost::format &format,
			  log_position &&pos) {
		const log_metadata metadata(
		    std::move(level),		    std::move(pos));
		write_to_log(std::move(metadata), boost::str(format));
	}

	inline void write(mlog_level &&level, boost::format &&format) {
		const log_metadata metadata(
		    std::move(level));
		write_to_log(std::move(metadata), boost::str(format));
	}

	inline void write(mlog_level &&level, const boost::format &format) {
		const log_metadata metadata(
		    std::move(level));
		write_to_log(std::move(metadata), boost::str(format));
	}

	inline void write(mlog_level &&level, std::string &&log_text,
			  log_position &&pos) {
		const log_metadata metadata(
		    std::move(level), std::move(pos));
		write_to_log(std::move(metadata), std::move(log_text));
	}

	inline void write(mlog_level &&level, std::string &&log_text) {
		const log_metadata metadata(
		    std::move(level));
		write_to_log(std::move(metadata), std::move(log_text));
	}

	template <typename T>
	void write(mlog_level &&level, const std::string &log_text, T &&pos) {
		const log_metadata metadata(
		    std::move(level), std::forward<T>(pos));
		write_to_log(std::move(metadata), std::string(log_text));
	}

	void write(mlog_level &&level, const std::string &log_text) {
		const log_metadata metadata(
		    std::move(level));
		write_to_log(std::move(metadata), std::string(log_text));
	}

	// virtual void flush() = 0;
	virtual void write_to_log(log_metadata &&metadata,
				  std::string &&log_text) = 0;

	virtual void write_to_log(const log_metadata &metadata,
				  const std::string &log_text) = 0;
};

template <typename T> struct logger : logger_base {

	virtual void write_to_log(log_metadata &&metadata,
				  std::string &&log_text) {
		static_cast<T *>(this)
		    ->write_to_log(std::move(metadata), std::move(log_text));
	}

	virtual void write_to_log(const log_metadata &metadata,
				  const std::string &log_text) {
		static_cast<T *>(this)->write_to_log(metadata, log_text);
	}
};
}
#endif /* LOGGER_HPP_ */

