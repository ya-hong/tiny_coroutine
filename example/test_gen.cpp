#include <iostream>
#include <string>

// #include "tiny_coroutine/detail/promise.hpp"
#include "tiny_coroutine/generator.hpp"
#include "tiny_coroutine/scheduler.hpp"
#include "tiny_coroutine/task.hpp"
#include "tiny_coroutine/utils.hpp"

using namespace tiny_coroutine;

generator<int> gen() {
	for (int i = 0; i < 5; ++i) {
		std::cout << "enter gen" << std::endl;
		co_yield i;
	}
	std::cout << "gen cleaning" << std::endl;
	co_return;
}

task<void> print_gen() {
	auto g = gen();
	while (!g.done()) {
		std::cout << "try print" << std::endl;
		std::cout << co_await g << std::endl;
	}
	co_return;
}

task<void> early_stop() {
	auto g = gen();
	int x = co_await g;
	std::cout << "early_stop" << std::endl;
	std::cout << x << std::endl;
	co_return;
}

int main() {
	scheduler s;
	s.spawn(print_gen);
	s.spawn(early_stop);
	s.join();
}
