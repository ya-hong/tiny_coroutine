#pragma once

#include <coroutine>

#include "handle.hpp"
#include "promise_state.hpp"
#include "scheduler_impl.hpp"

namespace tiny_coroutine {

namespace detail {

template <typename T>
class multi_entry_awaiter {
public:
	multi_entry_awaiter(multi_entry_handle<T> h) : waited_(h) {}

	bool await_ready() {
		return waited_.promise().state() == promise_state::Birth;
	}

	template <typename promise>
	void await_suspend(std::coroutine_handle<promise> waiter) {
		waited_.promise().set_parent(waiter);
	}

	T await_resume() {
		return waited_.promise().result();
	}

private:
	multi_entry_handle<T> waited_;
};

}  // namespace detail

}  // namespace tiny_coroutine
