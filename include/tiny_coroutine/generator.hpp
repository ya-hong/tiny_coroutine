#pragma once

#include "detail/multi_entry_awaiter.hpp"
#include "detail/handle.hpp"
#include "detail/scheduler_impl.hpp"
#include "detail/multi_entry_promise.hpp"

namespace tiny_coroutine {

namespace detail {

template <typename T>
class promise;

}

template <class T>
class generator {
	using handle = detail::multi_entry_handle<T>;
	using awaiter = detail::multi_entry_awaiter<T>;
	

public:
	using promise_type = detail::multi_entry_promise<T>;

	generator() = delete;

	generator(const generator<T>&) = delete;

	explicit generator(generator<T>&& other) : handle_(other.handle_) {
		other.handle_ = handle(nullptr);
	}

	generator& operator=(const generator<T>&) = delete;

	generator& operator=(generator<T>&& other) {
		std::swap(handle_, other.handle_);
	}

	bool done() const {
		return handle_.done();
	}


	generator(handle h) : handle_(h) {
		detail::scheduler_impl::local()->spawn_handle(handle_);
	}

	awaiter operator co_await() {
		return awaiter(handle_);
	}
	
	~generator() {
		// handle_.destroy();
	}

private:
	handle handle_;
};

}
