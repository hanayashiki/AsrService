#pragma once

#include <string>
#include <vector>

#include <libbson-1.0/bson.h>
#include "Log.h"

struct Speech {
public:
	bool Good = false;
	std::vector<uint8_t> Wave;
	std::string SpeechId;
private:
	static bool GetStringValue(bson_iter_t * sourceIter, 
		const char * keyName, std::string * str) {
		bson_iter_t target;
		bool success = bson_iter_find_descendant(sourceIter, keyName, &target);
		if (!success) return false;
		success = BSON_ITER_HOLDS_UTF8(&target);
		if (!success) return false;
		uint32_t len;
		*str = bson_iter_utf8(&target, &len);
		DEC_LOG << keyName << " is ok" << std::endl;
		return true;
	}

	static bool GetBinaryValue(bson_iter_t * sourceIter,
		const char * keyName, std::vector<uint8_t> * Wave) {
		bson_iter_t target;
		bool success = bson_iter_find_descendant(sourceIter, keyName, &target);
		if (!success) return false;
		DEC_LOG << "Find name " << keyName << " is ok" << std::endl;
		success = BSON_ITER_HOLDS_BINARY(&target);
		if (!success) return false;
		DEC_LOG << "Bson holds binary is ok" << std::endl;
		const uint8_t * binary;
		uint32_t len;
		bson_iter_binary(&target, NULL, &len, &binary);	
		*Wave = std::vector<uint8_t>(binary, binary + len);
		DEC_LOG << keyName << " is ok" << std::endl;
		return true;
	}	

public:
	Speech(const uint8_t * bsonData, size_t length) {
		bson_iter_t iter;
		if (bson_iter_init_from_data(&iter, bsonData, length) &&
			GetBinaryValue(&iter, "Wave", &Wave) &&
			GetStringValue(&iter, "SpeechId", &SpeechId)) {
			Good = true;
		}	
	}
};
