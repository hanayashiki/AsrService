#pragma once

#include "Speech.h"
#include "DecodeResult.h"


class DecoderCore {
public:
	virtual DecodeResult GetDecodeResult(Speech & speech) = 0;
};
