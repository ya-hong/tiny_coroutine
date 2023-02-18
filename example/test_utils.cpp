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

int main() {
	scheduler s;
	s.spawn(test_time_out);
	s.join();
}
