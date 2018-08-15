#include <thread>
#include <vector>
#include "../Include/Decoder.h"

int main() {
	std::vector<std::thread> threads;

	threads.emplace_back(Decoder::Run);
	

	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}

	return 0;
}
