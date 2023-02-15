#pragma once

#include "detail/awaiter.hpp"
#include "detail/handle.hpp"
#include "detail/promise.hpp"
#include "detail/scheduler_impl.hpp"

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
		detail::scheduler_impl::local()->spawn_handle(handle_);
	}

	awaiter operator co_await() {
		return awaiter(handle_);
	}

	~task() {
		// handle_.destroy();
	}

private:
	handle handle_;
};

}  // namespace tiny_coroutine
