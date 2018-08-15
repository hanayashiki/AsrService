#include <iostream>
#include <vector>

#include "../Include/Speech.h"
#include "../Include/DecodeResult.h"
#include "libbson-1.0/bson.h"


int main()
{
	bson_t * MockSpeech;
	uint8_t MockWave[] = { 1, 2, 3, 4, 5, 6, 7, 8 }; 
	MockSpeech = BCON_NEW(
			"SpeechId", BCON_UTF8("114514"), 
			"Wave", BCON_BIN(BSON_SUBTYPE_BINARY, MockWave, 8));
	Speech speech(bson_get_data(MockSpeech), MockSpeech->len);

	std::cout << "Good? " << speech.Good << std::endl;
	std::cout << "SpeechId == 114514 ? "<< speech.SpeechId << std::endl;
	std::cout << "Wave.size() == 8 ? " << speech.Wave.size() << std::endl;

	bson_destroy(MockSpeech);

	DecodeResult decode = { "114514", "Text", "Ok" };

	std::vector<uint8_t> bin = decode.ToBsonBin();

	bson_t * b = bson_new_from_data(bin.data(), bin.size()); 

	bson_iter_t iter;
	if (b) {
		if (bson_iter_init(&iter, b)) {
			while (bson_iter_next(&iter)) {
				uint32_t dummy;
				printf("DecodeResult %s : %s\n", bson_iter_key(&iter), 
					bson_iter_utf8(&iter, &dummy));
			}
		}	
	}

	bson_destroy(b);

	return 0;
}

