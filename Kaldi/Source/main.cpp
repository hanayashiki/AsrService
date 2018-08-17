#include <iostream>
#include <fstream>

#include "../Include/KaldiService.h"
#include "../Include/json.h"

int main(int argc, char * argv[]) {
	const char * usage = "kaldi-service <kaldi_config.json> <decoder_config.json>\n"
		"    <kaldi_config.json> configures basic service params,\n"
		"    <decoder_config.json> configures kaldi decoding parameters.\n";
	if (argc != 3) {
		std::cerr << usage << std::endl;
		return -1;
	}

	try {
		using nlohmann::json;
		json kaldi_config_j;
		std::ifstream kaldi_config_i(argv[1]);
		if (!kaldi_config_i.is_open()) {
			std::cerr << argv[0] << ": <kaldi_config.json> :\"" 
				<< argv[1] << "\" is not found." << std::endl;
			return -1;
		}
		kaldi_config_i >> kaldi_config_j; // input json
			
		DEC_LOG << "using kaldi_config.json: " << std::endl 
			<< std::setw(4) << kaldi_config_j << std::endl;
		
		std::ifstream decoder_config_i(argv[2]);
		json decoder_config_j;
		if (!decoder_config_i.is_open()) {
			std::cerr << argv[0] << ": <decoder_config.json> :\""
				<< argv[2] << "\" is not found." << std::endl;
		}
		decoder_config_i >> decoder_config_j;

		DEC_LOG << "using decoder_config.json: " << std::endl
			<< std::setw(4) << decoder_config_j << std::endl;

		KaldiServiceConfig kaldi_config = kaldi_config_j;
		kaldi::Online2WavNnet3LatgenDecoderConfig decoder_config 
			= decoder_config_j;

		KaldiService service(kaldi_config, decoder_config);
		
		DEC_LOG << "initializing service..." << std::endl;
		service.Run();

		return -1;
	} catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;


}
