#pragma once

#include <string>

struct KaldiServiceConfig {
	bool use_kaldi;
	int workers;
	std::string redis_key;
	std::string redis_server;
	int redis_port;
	size_t max_wave_size_byte;

	bool thread_expires_after_decoding;
};
