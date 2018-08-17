#pragma once

#include <online2/online2-wav-nnet3-latgen-faster-api.h>
#include <streambuf>
#include <istream>

#include "json.h"
#include "DecodeResult.h"
#include "DecoderCore.h"

namespace kaldi {
	static void to_json(nlohmann::json & j, const Online2WavNnet3LatgenDecoderConfig & c) {
		j = nlohmann::json{
			{ "chunk_length_secs", c.chunk_length_secs },
			{ "do_endpointing", c.do_endpointing },
			{ "online", c.online },
			{ "num_threads_startup", c.num_threads_startup },
			{ "feature_type", c.feature_type },
			{ "config", c.config },
			{ "add_pitch", c.add_pitch },
			
			{ "max_active", c.max_active },
			{ "beam", c.beam },
			{ "lattice_beam", c.lattice_beam },
			{ "frames_per_chunk", c.frames_per_chunk },
			{ "acoustic_scale", c.acoustic_scale },
			
			{ "word_symbol_table", c.word_symbol_table },
			{ "nnet3_in", c.nnet3_in },
			{ "fst_in", c.fst_in },
			{ "lattice_wspecifier", c.lattice_wspecifier },
			
			{ "sample_freq", c.sample_freq }
		};
	}

	static void from_json(const nlohmann::json & j, Online2WavNnet3LatgenDecoderConfig & c) {
		c.chunk_length_secs = j.at("chunk_length_secs").get<float>();
		c.do_endpointing = j.at("do_endpointing").get<bool>();
		c.online = j.at("online").get<bool>();
		c.num_threads_startup = j.at("num_threads_startup").get<int>();
		c.feature_type = j.at("feature_type").get<std::string>();
		c.config = j.at("config").get<std::string>();
		c.add_pitch = j.at("add_pitch").get<bool>();

		c.max_active = j.at("max_active").get<int>();
		c.beam = j.at("beam").get<float>();
		c.lattice_beam = j.at("lattice_beam").get<float>();
		c.frames_per_chunk = j.at("frames_per_chunk").get<int>();
		c.acoustic_scale = j.at("acoustic_scale").get<float>();

		c.word_symbol_table = j.at("word_symbol_table").get<std::string>();
		c.nnet3_in = j.at("nnet3_in").get<std::string>();
		c.fst_in = j.at("fst_in").get<std::string>();
		c.lattice_wspecifier = j.at("lattice_wspecifier").get<std::string>();
		
		c.sample_freq = j.at("sample_freq").get<float>();
	}
}

class KaldiDecoderCore : public DecoderCore {
public:
	/* Thread-safe? decoding */
	virtual DecodeResult GetDecodeResult(Speech & speech) override;
	/* This will initialize a KaldiDecoderCore, which should be called
	 * ONLY ONCE, 
	 * which will take long to grab all the resource that a
	 * `kaldi::Online2WavNnet3LatgenDecoder` needs;
	 * */
	static KaldiDecoderCore * Instance
		(kaldi::Online2WavNnet3LatgenDecoderConfig & config);
	/* Simple passes the singleton Instance */
	static KaldiDecoderCore * Instance();
	/* This is called to delete the singleton 
	 * `static KaldiDecoderCore * instance`
	 * */
	static void Finalize();
private:
	/* membuf, imemstream from snippet of 
	 * https://stackoverflow.com/questions/13059091/
	 * 	creating-an-input-stream-from-constant-memory */
	struct membuf: std::streambuf {
		membuf(char const* base, size_t size) {
			char* p(const_cast<char*>(base));
		 	this->setg(p, p, p + size);
		}
	};
	struct imemstream: virtual membuf, std::istream {
		imemstream(char const* base, size_t size)
		: membuf(base, size) , std::istream(static_cast<std::streambuf*>(this))
		{
		}
	};
	KaldiDecoderCore(kaldi::Online2WavNnet3LatgenDecoder * decoder);
	kaldi::Online2WavNnet3LatgenDecoder * decoder = nullptr;

	static KaldiDecoderCore * instance;
};
