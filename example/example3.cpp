#include <iostream>
#include <string>

#include "tiny_coroutine.hpp"

using namespace tiny_coroutine;

generator<int> gen(int x) {
	while (true) {
		co_yield x++;
	}
}

task<void> print() {
	auto g = gen(5);
	for (int i = 0; i < 3 && !g.done(); ++i) {
		std::cout << co_await g << std::endl;
	}
	co_return;
}

int main() {
	scheduler s;
	s.spawn(print);
	s.join();
	return 0;
}
