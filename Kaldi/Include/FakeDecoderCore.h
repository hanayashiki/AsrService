#pragma once
#include "DecoderCore.h"

class FakeDecoderCore : public DecoderCore {
public:
	virtual DecodeResult GetDecodeResult(Speech & speech) override {
		return { speech.SpeechId, "fake result", "Ok kana?" };		
	}
};
