#include <thread>
#include <iostream>
#include <vector>

int f1(int n) {
	for (int i = 0; i < 5; i++) {
		std::cout << "thread [" << n << "]" 
			<< " loops for " << i << " time(s)" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

int main()
{
	std::vector<std::thread> threads;
	for (int i = 0; i < 3; i++) {
		threads.emplace_back(f1, i+1);
	}
	for (int i = 0; i < 3; i++) {
		threads[i].join();
	}
}
