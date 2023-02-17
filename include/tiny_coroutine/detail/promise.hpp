#pragma once

#include <coroutine>

#include "handle.hpp"
#include "promise_state.hpp"
#include "scheduler_impl.hpp"
#include "store.hpp"
#include "tiny_coroutine/task.hpp"

namespace tiny_coroutine {

template <typename T>
class task;

namespace detail {

class promise_no_type {
public:
	promise_no_type() : parent_handle_(nullptr) {}

	std::suspend_always initial_suspend() noexcept {
		return {};
	}

	auto final_suspend() noexcept {
		struct awaiter {
			awaiter(promise_no_type* p) : p(p) {}
			bool await_ready() noexcept {
				return (bool)p->parent_handle();
			}
			void await_suspend(std::coroutine_handle<> handle) noexcept {
				scheduler_impl::local()->spawn_handle(p->parent_handle());
			}
			void await_resume() noexcept {}
			promise_no_type* p;
		};
		return awaiter(this);
	}

	std::coroutine_handle<>& parent_handle() noexcept {
		return parent_handle_;
	}

	void unhandled_exception() {
		// TODO
	}

private:
	std::coroutine_handle<> parent_handle_;
};

template <typename T>
class promise : public promise_no_type {
public:
	promise() : parent_handle_(nullptr), state_(promise_state::PREGNANCY) {}

	task<T> get_return_object() noexcept {
		return task<T>(handle<T>::from_promise(*this));
	}

	void return_value(T&& value) {
		store_.write(std::forward<T>(value));
		transfer_write(state_);
	}

	T result() {
		transfer_read(state_);
		return store_.read();
	}

	void mark_cancel() noexcept {
		transfer_cancel(state_);
	}

	promise_state state() const noexcept {
		return state_;
	}

	~promise() noexcept {
		transfer_cancel(state_);
		if (state_ == promise_state::ABANDON) {
			store_.erase();
		}
	}

private:
	std::coroutine_handle<> parent_handle_;
	promise_state state_;
	store<T> store_;
};

template <>
class promise<void> : public promise_no_type {
public:
	promise() : parent_handle_(nullptr), state_(promise_state::PREGNANCY) {}

	task<void> get_return_object() noexcept {
		return task<void>(handle<void>::from_promise(*this));
	}

	void return_void() {}

	void result() {}

	void mark_cancel() noexcept {
		transfer_cancel(state_);
	}

	promise_state state() const noexcept {
		return state_;
	}

private:
	std::coroutine_handle<> parent_handle_;
	promise_state state_;
};

}  // namespace detail

}  // namespace tiny_coroutine
