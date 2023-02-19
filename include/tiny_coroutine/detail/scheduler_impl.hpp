#pragma once

#include <cassert>
#include <coroutine>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <type_traits>

#include "promise_state.hpp"
// #include "promis_no_type.hpp"

namespace tiny_coroutine {

namespace detail {

class promise_no_type;

class scheduler_impl {
public:
	scheduler_impl() : try_join_(false) {
		host_thread_ = std::thread([this]() {
			local_scheduler_pimpl_ = this;
			this->loop_();
		});
	}

	void join() {
		try_join_ = true;
		host_thread_.join();
	}

	// template <typename promise_type>
	void spawn_lambda(std::function<void()> lambda) {
		std::lock_guard<std::mutex> lg{mutex_};
		fragment_queue_.push(lambda);
		// fragment_queue_.push([promise_ptr]() {
		// 	auto handle = std::coroutine_handle<>::from_address(promise_ptr);
		// 	if (promise_ptr->state() == promise_state::ABANDON ||
		// promise_ptr->state() == promise_state::ABORTION) {
		// handle.destroy();
		// 	}
		// 	else {
		// 		handle.resume();
		// 	}
		// });
	}

	static scheduler_impl* local() {
		assert(scheduler_impl::local_scheduler_pimpl_ != nullptr &&
			   "no local scheduler in this thread!");
		return scheduler_impl::local_scheduler_pimpl_;
	}

private:
	void loop_() {
		while (!try_join_ || !fragment_queue_.empty()) {
			if (fragment_queue_.empty()) continue;
			std::function<void()> fragment;
			{
				std::lock_guard<std::mutex> lg{mutex_};
				fragment = fragment_queue_.front();
				fragment_queue_.pop();
			}
			fragment();
		}
	}

	std::mutex mutex_;
	std::queue<std::function<void()>> fragment_queue_;
	bool try_join_;
	std::thread host_thread_;
	inline static thread_local scheduler_impl* local_scheduler_pimpl_;
};

}  // namespace detail

}  // namespace tiny_coroutine
