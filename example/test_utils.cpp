#include <iostream>
#include <string>

#include "tiny_coroutine/generator.hpp"
#include "tiny_coroutine/scheduler.hpp"
#include "tiny_coroutine/task.hpp"
#include "tiny_coroutine/utils.hpp"

using namespace tiny_coroutine;

task<int> get_value() {
	co_await sleep(5);
	co_return 114514;
}

task<void> test_time_out() {
	auto op = co_await timeout(get_value(), 2);
	if (op) {
		std::cout << "getvalue " << *op << std::endl;
	} else {
		std::cout << "timeout" << std::endl;
	}
}

task<void> test_time_out2() {
	auto t = get_value();
	co_await when_any(t, sleep(2));
	if (t.done()) {
		std::cout << "getvalue " << co_await t << std::endl;
	} else {
		std::cout << "timeout" << std::endl;
	}
}

task<void> test_time_out3() {
	auto t = get_value();
	co_await when_all(t, sleep(2));
	if (t.done()) {
		std::cout << "getvalue " << co_await t << std::endl;
	} else {
		std::cout << "timeout" << std::endl;
	}
	co_return;
}

int main() {
	scheduler s;
	s.spawn(test_time_out);
	s.spawn(test_time_out2);
	s.spawn(test_time_out3);
	s.join();
}
