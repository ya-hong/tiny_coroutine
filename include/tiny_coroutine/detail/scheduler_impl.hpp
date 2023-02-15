#pragma once

#include <cassert>
#include <coroutine>
#include <functional>
#include <iostream>
#include <queue>
#include <thread>
#include <type_traits>

namespace tiny_coroutine {

namespace detail {

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

	void spawn_handle(std::coroutine_handle<> h) {
		std::lock_guard<std::mutex> lg{mutex_};
		fragment_queue_.push([h]() { h.resume(); });
	}

	template <typename _Callable, typename... _Args>
	void spawn(_Callable&& __f, _Args&&... __args) {
		// static_assert(
		// 	std::invocable<typename decay<_Callable>::type, typename
		// decay<_Args>::type...>, 	"std::thread arguments must be invocable
		// after conversion to rvalues"
		// );

		std::lock_guard<std::mutex> lg{mutex_};
		fragment_queue_.push(
			[__f{std::forward<_Callable>(__f)},
			 ... __args{std::forward<_Args>(__args)}]() mutable {
				__f(std::forward<_Args>(__args)...);
			});
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