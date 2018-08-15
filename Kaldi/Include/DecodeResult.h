#include <string>
#include <utility>
#include <libbson-1.0/bson.h>

class DecodeResult {
public:
	std::string Id;
	std::string Text;
	std::string Message;

	std::vector<uint8_t> ToBsonBin() {
		bson_t b = BSON_INITIALIZER;
		BSON_APPEND_UTF8(&b, "Id", Id.c_str());
		BSON_APPEND_UTF8(&b, "Text", Text.c_str());
		BSON_APPEND_UTF8(&b, "Message", Message.c_str());
		const uint8_t * data_start = bson_get_data(&b);
		std::vector<uint8_t> data(data_start, data_start + b.len);
		bson_destroy(&b);
		return std::move(data);
	}
};
