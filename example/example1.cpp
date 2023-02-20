#include <iostream>
#include <string>

#include "tiny_coroutine.hpp"

using namespace tiny_coroutine;

task<void> enter() {
	std::cout << "enter coroutine" << std::endl;
	co_return;
}

task<void> hello() {
	co_await timeout(2);
	std::cout << "hello coroutine" << std::endl;
	co_return;
}

int main() {
	scheduler s;
	s.spawn(enter);
	s.spawn(hello);
	s.join();
	return 0;
}
