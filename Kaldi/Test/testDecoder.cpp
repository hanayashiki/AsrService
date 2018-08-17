#include <thread>
#include <vector>
#include "../Include/Decoder.h"
#include "../Include/DecoderCore.h"
#include "../Include/FakeDecoderCore.h"

int main() {
	
	std::vector<std::thread> threads;

	FakeDecoderCore fakeCore;
	threads.emplace_back(Decoder::Run, &fakeCore);
	

	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}

	return 0;
}
