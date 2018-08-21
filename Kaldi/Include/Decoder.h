#include <iostream>
#include <csignal>
#include <cstdlib>
#include <ctime>
#include <string>
#include <thread>

#include "Log.h"
#include "DecodeResult.h"
#include "Speech.h"
#include "hiredis/hiredis.h"
#include "hiredis/async.h"
#include "hiredis/adapters/libevent.h"
#include "DecoderCore.h"
#include "KaldiServiceConfig.h"


/* This is the manager of resources and actions corresponding to
 * a decoding worker.
 *
 * TODO: introduce real decoder Kaldi
 * */

class Decoder {
public:
	static inline void Run(DecoderCore * core, KaldiServiceConfig && config) {
		Decoder decoder(core, config);
		decoder.run();
	}
private:
	const char * SpeechSignal = "SpeechSignal";
	const char * SpeechQueue = "SpeechQueue";
	const char * TextSignal = "TextSignal";
	const char * TextQueue = "TextQueue";	

	struct event_base * eventBase = nullptr;
       	redisAsyncContext * asyncContext = nullptr;
	redisContext * context = nullptr;
	DecoderCore * core;
	KaldiServiceConfig & config;

	Decoder(DecoderCore * core, KaldiServiceConfig & config)
		: eventBase(event_base_new()),
		core(core),
		config(config)	
	{
		
	}
	~Decoder()
	{
		if (eventBase) event_base_free(eventBase);
		if (asyncContext) redisAsyncDisconnect(asyncContext);
		if (context) redisFree(context);
	}

    	static void connect_cb(const redisAsyncContext *ac, int status) {
		DEC_LOG << "connect status: " << status << std::endl;
	}

	static bool connectToRedis(
		redisAsyncContext * & ac, redisContext * & c, 
		const char * ip, int port) {	
		ac = redisAsyncConnect(ip, port);
		redisAsyncSetConnectCallback(ac, connect_cb);
		c = redisConnectWithTimeout(ip, port, { 3, 0 });
		if (c == nullptr || c->err || ac == nullptr || ac->err) {
			DEC_LOG << "cannot connect to redis " << ip << 
				":" << port << std::endl; 
			return false;
		}
		return true;
	}
	static void onMessage(redisAsyncContext * c, void * reply, void * privdata) {
		/* 
		 * privdata: Decoder * this
		 * */
		redisReply * r = (redisReply*)reply;
		if (r == nullptr) return;
		if (r->type == REDIS_REPLY_ARRAY &&
			r->elements == 3 &&
			strcmp(r->element[0]->str, "message") == 0) {
			Decoder * decoder = (Decoder *)privdata;
			DecodeResult result;
			bool success = decoder->fetchSpeech(&result);
			if (!success) {
				//DEC_LOG << "fail to fetch Speech";
				return;		
			}
			auto data = result.ToBsonBin();
			DEC_LOG << "result message: " << result.Message << std::endl;
			r = (redisReply*)redisCommand(decoder->context, 
				"LPUSH %s %b", decoder->TextQueue, 
				data.data(), data.size());
			freeReplyObject(r);
			r = (redisReply*)redisCommand(decoder->context,
				"PUBLISH %s \"\"", decoder->TextSignal);
			freeReplyObject(r);

			if (decoder->config.thread_expires_after_decoding) {
				DEC_LOG << "exiting..." << std::endl;
				redisAsyncCommand(decoder->asyncContext, NULL, NULL,
					"UNSUBSCRIBE %s", decoder->SpeechSignal);
				redisAsyncDisconnect(decoder->asyncContext);	
				decoder->asyncContext = nullptr;
			}
		}
			
	}
	bool fetchSpeech(DecodeResult * result) {
		redisReply * reply;
		reply = (redisReply*)redisCommand(context, "RPOP %s", SpeechQueue);
		// DEC_LOG << "fetchSpeech called" << std::endl;
		if (reply == nullptr || reply->type != REDIS_REPLY_STRING) {
			if (reply) freeReplyObject(reply);
			return false;
		}
		const uint8_t * data = (const uint8_t*)reply->str;
		size_t len = reply->len;
		Speech speech(data, len);
		freeReplyObject(reply);
		if (!speech.Good) {
			DEC_LOG << "This speech is not good";
			DEC_LOG << ", len: " << len << std::endl; 
			return false;
		}
		result->Id = speech.SpeechId;
		if (speech.Wave.size() > config.max_wave_size_byte) {
			result->Message = 
				std::string("The wave file should be smaller than ") + 
				std::to_string(config.max_wave_size_byte) + " bytes";
			result->Text = "";
			return true;
		}

		*result = core->GetDecodeResult(speech);
		
		return true;
	}
	void run() {
		//signal(SIGPIPE, SIG_IGN);
		bool success;
		// TODO: from config
		success = connectToRedis(asyncContext, context, config.redis_server.c_str(),
			config.redis_port); 
		if (!success) {
			DEC_LOG << "connection not succeeded" << std::endl;
			return;
		}
		// Into event loop
		redisLibeventAttach(asyncContext, eventBase);
		redisAsyncCommand(asyncContext, onMessage, this,
				"SUBSCRIBE %s", SpeechSignal);
		DEC_LOG << std::this_thread::get_id() << ": " << "Decoder listening..." << std::endl;
		event_base_dispatch(eventBase);
	}

};
