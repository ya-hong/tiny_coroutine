#include <iostream>
#include <string>

// #include "tiny_coroutine/detail/promise.hpp"
#include "tiny_coroutine/generator.hpp"
#include "tiny_coroutine/scheduler.hpp"
#include "tiny_coroutine/task.hpp"
#include "tiny_coroutine/utils.hpp"

using namespace tiny_coroutine;

int x = 0;

task<int&> return_reference() {
	std::cout << "enter return_reference" << std::endl;
	co_return x;
}

task<void> update() {
	std::cout << "enter update" << std::endl;
	auto&& x = co_await return_reference();
	std::cout << "=> update x" << std::endl;
	x += 1;
	std::cout << x << std::endl;
	co_return;
}

task<void> update_loop() {
	for (int i = 0; i < 20; ++i) {
		std::cout << "update loop " << i << std::endl;
		co_await update();
		std::cout << "=>updated" << std::endl;
	}
	co_return;
}

int main() {
	scheduler s;
	s.spawn(update_loop);
	s.join();
	std::cout << x << std::endl;
}
