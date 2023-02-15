#include <iostream>
#include <string>

#include "tiny_coroutine/detail/promise.hpp"
#include "tiny_coroutine/generator.hpp"
#include "tiny_coroutine/scheduler.hpp"
#include "tiny_coroutine/task.hpp"
#include "tiny_coroutine/utils.hpp"

using namespace tiny_coroutine;

template <class T>
task<void> print(T x) {
	std::cout << x << std::endl;
	co_return;
}

task<int> fib(int n) {
	std::cout << "==> " << n << std::endl;
	if (n <= 2) {
		co_return 1;
	}
	co_return co_await fib(n - 1) + co_await fib(n - 2);
}

task<void> f() {
	co_await print(co_await fib(10));
	co_return;
}

int x = 0;

task<int&> return_reference() {
	co_return x;
}

task<void> update() {
	auto&& x = co_await return_reference();
	x += 1;
	co_return;
}

task<void> update_loop() {
	for (int i = 0; i < 20; ++i) {
		co_await update();
	}
	co_return;
}

task<void> check() {
	auto&& x = co_await return_reference();
	co_await tiny_coroutine::condition([&x]() mutable { return x > 5; });
	print(x);
	co_return;
}

generator<int> gen() {
	for (int i = 0; i < 5; ++i) {
		std::cout << "enter gen" << std::endl;
		co_yield i;
	}
	co_return;
}

task<void> print_gen() {
	auto g = gen();
	while (!g.done()) {
		std::cout << co_await g << std::endl;
	}
	co_return;
}

int main() {
	scheduler s;
	// s.spawn(check);
	// s.spawn(update_loop);
	s.spawn(print_gen);
	s.join();
	// std::cout << x << std::endl;
}