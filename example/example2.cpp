#include <iostream>
#include <optional>
#include <string>

#include "tiny_coroutine.hpp"

using namespace tiny_coroutine;

task<int> hard_work() {
	co_await timeout(5);
	co_return 114514;
}

task<int> simple_work() {
	co_await timeout(2);
	co_return 19260817;
}

task<void> task1() {
	std::optional<int> result = co_await timeout(hard_work(), 3);
	if (!result) {
		std::cout << "task1: hard work time out" << std::endl;
	} else {
		std::cout << "task1: hard word result " << *result << std::endl;
	}
}

task<void> task2() {
	auto w1 = hard_work();
	auto w2 = simple_work();
	co_await timeout(when_all(w1, w2), 3);
	if (w1.done()) {
		std::cout << "task2: hard work result " << co_await w1 << std::endl;
	}
	if (w2.done()) {
		std::cout << "task2: simple work result " << co_await w2 << std::endl;
	}
}

int main() {
	scheduler s;
	s.spawn(task1);
	s.spawn(task2);
	s.join();
	return 0;
}
