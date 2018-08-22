#pragma once

#include <iostream>
#include <string>
#include <ctime>

#define _DEC_LOG(x) std::cerr << "[" << #x << "] " << __FUNCTION__ << "@[" << \
			__get_filename(__FILE__) << ", " << __LINE__ << ", " << __now() <<  "] "

#define DEC_LOG _DEC_LOG(INFO)

#define DEC_DEBUG _DEC_LOG(DEBUG)

#define DEC_WARN _DEC_LOG(WARN)
#define DEC_ERROR _DEC_LOG(DEC_ERROR)
#define DEC_FATAL _DEC_LOG(FATAL)

static inline std::string __now() {
	time_t now = time(0);
	std::string date = ctime(&now);
	for (int i = date.length() - 1; i >= 0; i--) {
		if (std::isspace(date[i])) {
			date.pop_back();
		} else {
			return date;
		}
	}
}

static inline std::string __get_filename(std::string path) { 
	int i;
	for (i = path.length() - 1; i >= 0; i--) {
		if (path[i] == '/' || path[i] == '\\') {
			break;
		}
	}
	return path.substr(i+1);
}
