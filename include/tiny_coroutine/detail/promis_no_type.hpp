/**
 * 擦除类型的promise
 * 作为基类用于派生task_promise与generator_promise
 */

#pragma once

#include <coroutine>

#include "handle.hpp"
#include "promise_state.hpp"
#include "scheduler_impl.hpp"
#include "store.hpp"

namespace tiny_coroutine {

namespace detail {

class promise_no_type {
public:
	promise_no_type(co_handle self)
		: listener_(nullptr), state_(promise_state::Pregnancy), self_(self) {}

	void schedule_awake() {
		scheduler_impl::local()->spawn_lambda([this]() {
			if (this->state_ != promise_state::Abort) [[likely]] {
				this->get_co_handle().resume();
			}
		});
	}

	void schedule_destory() {
		scheduler_impl::local()->spawn_lambda(
			[this]() { this->get_co_handle().destroy(); });
	}

	bool try_schedule_awake_parent() {
		if (listener_ == nullptr || listener_->state() == promise_state::Abort)
			return false;
		listener_->schedule_awake();
		listener_ = nullptr;
		return true;
	}

	std::suspend_always initial_suspend() noexcept {
		return {};
	}

	std::suspend_always final_suspend() noexcept {
		return {};
	}

	void set_listener(pr_handle listener) noexcept {
		listener_ = listener;
	}

	pr_handle get_listener_handle() noexcept {
		return listener_;
	}

	std::coroutine_handle<> get_co_handle() noexcept {
		return self_;
	}

	void unhandled_exception() {
		// TODO
		std::rethrow_exception(std::current_exception());
	}

	promise_state state() const noexcept {
		return state_;
	}

protected:
	pr_handle listener_;
	co_handle self_;
	promise_state state_;
};

}  // namespace detail

}  // namespace tiny_coroutine

#include <vector>
