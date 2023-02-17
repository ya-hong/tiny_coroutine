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
	promise_no_type(std::coroutine_handle<> handle)
		: parent_ptr_(nullptr),
		  state_(promise_state::Pregnancy),
		  handle_(handle) {}

	void schedule_awake() {
		scheduler_impl::local()->spawn_lambda([this]() {
			if (this->state_ == promise_state::Abort) [[unlikely]] {
				this->get_handle().destroy();
			} else {
				if (!this->get_handle().done()) {
					this->get_handle().resume();
				} else {
					this->get_handle().destroy();
				}
			}
		});
	}

	void try_schedule_awake_parent() {
		if (parent_ptr_ == nullptr) return;
		parent_ptr_->schedule_awake();
		parent_ptr_ = nullptr;
	}

	std::suspend_always initial_suspend() noexcept {
		return {};
	}

	auto final_suspend() noexcept {
		return std::suspend_always{};
		// struct awaiter {
		// 	awaiter(promise_no_type* p) : p(p) {}
		// 	bool await_ready() noexcept {
		// 		return (bool)p->get_parent_ptr() && p->state_ ==
		// promise_state::BIRTH;
		// 	}
		// 	void await_suspend(std::coroutine_handle<> handle) noexcept {
		// 		p->schedule_awake(handle);
		// 	}
		// 	void await_resume() noexcept {}
		// 	promise_no_type* p;
		// };
		// return awaiter(this);
	}

	template <typename promise>
	void set_parent(std::coroutine_handle<promise> handle) noexcept {
		parent_ptr_ = &handle.promise();
	}

	promise_no_type* get_parent_ptr() noexcept {
		return parent_ptr_;
	}

	std::coroutine_handle<> get_handle() noexcept {
		return handle_;
	}

	void unhandled_exception() {
		// TODO
		std::rethrow_exception(std::current_exception());
	}

	promise_state state() const noexcept {
		return state_;
	}

protected:
	promise_no_type* parent_ptr_;
	std::coroutine_handle<> handle_;
	promise_state state_;
};

}  // namespace detail

}  // namespace tiny_coroutine

#include <vector>
