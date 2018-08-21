#pragma once

#include <string>
#include <thread>
#include <vector>

#include "Decoder.h"
#include "FakeDecoderCore.h"
#include "KaldiDecoderCore.h"
#include "Speech.h"
#include "DecodeResult.h"

#include "json.h"
#include "KaldiServiceConfig.h"

static void to_json(nlohmann::json & j, const KaldiServiceConfig & c) {
	using nlohmann::json;
	j = json{
		{ "workers", c.workers },
		
		{ "redis_key", c.redis_key },
		{ "redis_server", c.redis_server },
		{ "redis_port", c.redis_port },
		
		{ "max_wave_size_byte", c.max_wave_size_byte },
		{ "thread_expires_after_decoding", c.thread_expires_after_decoding }
	};
}

static void from_json(const nlohmann::json & j, KaldiServiceConfig & c) {
	using nlohmann::json;
	c.use_kaldi = j.at("use_kaldi").get<bool>();
	c.workers = j.at("workers").get<int>();
	c.redis_key = j.at("redis_key").get<std::string>();
	c.redis_server = j.at("redis_server").get<std::string>();
	c.redis_port = j.at("redis_port").get<int>();

	c.max_wave_size_byte = j.at("max_wave_size_byte").get<int>();
	c.thread_expires_after_decoding = j.at("thread_expires_after_decoding").get<bool>();
}



class KaldiService {
private:
	std::vector<std::thread> workers;
	KaldiServiceConfig & config;
	kaldi::Online2WavNnet3LatgenDecoderConfig & decoder_config;
public:
	KaldiService(KaldiServiceConfig & config,
		kaldi::Online2WavNnet3LatgenDecoderConfig & decoder_config)
		: config(config), decoder_config(decoder_config) {
	}

	void Run() {
		DecoderCore * core;
		if (config.use_kaldi) {
			core = KaldiDecoderCore::Instance(decoder_config);
		} else {
			core = new FakeDecoderCore();
		}
		
		for (int i = 0; i < config.workers; i++) {
			workers.push_back(std::thread(Decoder::Run, core, config));
			//workers.emplace_back(Decoder::Run, core, config);
		} 

		for (int i = 0; i < workers.size(); i++) {
			workers[i].join();
		}

		if (config.use_kaldi) {
			KaldiDecoderCore::Finalize();	
		} else {
			delete core;
		}
	}

};
