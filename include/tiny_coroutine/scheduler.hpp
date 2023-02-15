#pragma once

#include <cassert>
#include <coroutine>
#include <thread>

#include "detail/scheduler_impl.hpp"

namespace tiny_coroutine {
using detail::scheduler_impl;

class scheduler {
public:
	scheduler() : scheduler_pimpl_(new scheduler_impl) {}

	scheduler(const scheduler&) = delete;

	explicit scheduler(scheduler&& other)
		: scheduler_pimpl_(other.scheduler_pimpl_) {
		other.scheduler_pimpl_ = nullptr;
	}

	scheduler& operator=(const scheduler&) = delete;

	scheduler& operator=(scheduler&& other) {
		std::swap(scheduler_pimpl_, other.scheduler_pimpl_);
		return *this;
	}

	void join() {
		scheduler_pimpl_->join();
	}

	template <typename _Callable, typename... _Args>
	void spawn(_Callable&& __f, _Args&&... __args) {
		scheduler_pimpl_->spawn(std::forward<_Callable>(__f),
								std::forward<_Args>(__args)...);
	}

private:
	scheduler_impl* scheduler_pimpl_;
};

}  // namespace tiny_coroutine
