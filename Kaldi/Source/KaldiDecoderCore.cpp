
#include "../Include/KaldiDecoderCore.h"
#include "../Include/Log.h"
#include <cassert>

KaldiDecoderCore * KaldiDecoderCore::instance = nullptr;

KaldiDecoderCore * KaldiDecoderCore::
	Instance(kaldi::Online2WavNnet3LatgenDecoderConfig & config) {
	assert(instance == nullptr);
	DEC_LOG << "Initializing kaldi::Online2WavNnet3LatgenDecoder... " << std::endl;
	DEC_LOG << "This may fucking take 2 minutes." << std::endl;
	auto decoder = new kaldi::Online2WavNnet3LatgenDecoder(config);
	instance = new KaldiDecoderCore(decoder);
	return Instance();
}

KaldiDecoderCore * KaldiDecoderCore::Instance() {
	assert(instance != nullptr);
	return instance;
}

KaldiDecoderCore::KaldiDecoderCore(kaldi::Online2WavNnet3LatgenDecoder * decoder)
	: decoder(decoder) {
}

DecodeResult KaldiDecoderCore::GetDecodeResult(Speech & speech) {
	std::istream && wave_stream = imemstream(
		(char *)speech.Wave.data(), 
		speech.Wave.size());
	kaldi::DecodingResult kaldi_result = decoder->GetResult(
		speech.SpeechId, wave_stream);
	DecodeResult result;
	if (kaldi_result.good) {
		result.Message = "Ok";
	} else { 
		result.Message = kaldi_result.message;
	}
	result.Id = speech.SpeechId;
	result.Text = kaldi_result.text;

	return result;
}

void KaldiDecoderCore::Finalize() {
	if (instance != nullptr) {
		delete instance->decoder;
		delete instance;
	}
}
