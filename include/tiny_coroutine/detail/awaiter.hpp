#pragma once

#include <coroutine>

#include "handle.hpp"
#include "scheduler_impl.hpp"

namespace tiny_coroutine {

namespace detail {

template <typename T>
class awaiter {
public:
	awaiter(handle<T> h) : waited_(h) {}

	bool await_ready() {
		return waited_.done();
	}

	void await_suspend(std::coroutine_handle<> waiter) {
		waited_.promise().parent_handle() = waiter;
	}

	T await_resume() {
		return waited_.promise().result();
	}
	
private:
	handle<T> waited_;
};

}

}