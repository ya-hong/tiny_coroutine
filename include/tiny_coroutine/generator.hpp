#pragma once

#include "detail/awaiter.hpp"
#include "detail/handle.hpp"
#include "detail/multi_entry_promise.hpp"
#include "detail/promise_state.hpp"
#include "detail/scheduler_impl.hpp"

namespace tiny_coroutine {

template <class T>
class generator {
public:
	using promise_type = detail::multi_entry_promise<T>;
	using awaiter = detail::awaiter<promise_type>;

	generator() = delete;

	generator(const generator&) = delete;

	explicit generator(generator&& other) : pr_handle_(other.pr_handle_) {
		other.pr_handle_ = nullptr;
	}

	generator& operator=(const generator&) = delete;

	generator& operator=(generator&& other) {
		std::swap(pr_handle_, other.pr_handle_);
	}

	generator(promise_type* h) : pr_handle_(h) {
		pr_handle_->schedule_awake();
	}

	bool done()
		const {	 // this genenrator<> manage a coroutine and coroutine done
		return pr_handle_ && pr_handle_->get_co_handle().done();
	}

	bool await_ready() const {
		if (pr_handle_ == nullptr) [[unlikely]] {
			throw "this generator<> no longer manages the lifetime of coroutines";
		}
		return pr_handle_->state() == detail::promise_state::Birth;
	}

	awaiter operator co_await() const {
		return awaiter{pr_handle_};
	}

	~generator() {
		pr_handle_->abort();
	}

private:
	promise_type* pr_handle_;
};

}  // namespace tiny_coroutine
