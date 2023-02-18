#pragma once

#include "detail/awaiter.hpp"
#include "detail/handle.hpp"
#include "detail/promise_state.hpp"
#include "detail/scheduler_impl.hpp"
#include "detail/task_promise.hpp"

namespace tiny_coroutine {

namespace detail {

template <typename T>
class promise;

}

template <class T>
class task {
	using handle = detail::handle<T>;
	using awaiter = detail::awaiter<T>;

public:
	using promise_type = detail::promise<T>;

	task() = delete;

	task(const task<T>&) = delete;

	explicit task(task<T>&& other) : handle_(other.handle_) {
		other.handle_ = handle(nullptr);
	}

	task& operator=(const task<T>&) = delete;

	task& operator=(task<T>&& other) {
		std::swap(handle_, other.handle_);
	}

	task(handle h) : handle_(h) {
		handle_.promise().schedule_awake();
	}

	awaiter operator co_await() {
		return awaiter(handle_);
	}

	bool await_ready() const {
		return (bool)handle_ && handle_.done();
	}

	void abort() {
		if (!handle_) {
			throw "this task<> no longer manages the lifetime of coroutines";
		}
		handle_.promise().abort();
		handle_ = handle(nullptr);
	}

	void detach() {
		if (!handle_) {
			throw "this task<> no longer manages the lifetime of coroutines";
		}
		handle_.promise().detach();
		handle_ = handle(nullptr);
	}

	~task() {
		if (handle_) {
			detach();
		}
	}

private:
	handle handle_;
};

}  // namespace tiny_coroutine
