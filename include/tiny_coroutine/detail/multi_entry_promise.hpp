#pragma once

#include <coroutine>

#include "handle.hpp"
#include "promise_state.hpp"
#include "scheduler_impl.hpp"
#include "store.hpp"

namespace tiny_coroutine {

template <typename T>
class generator;

namespace detail {

template <typename T>
class multi_entry_promise {
public:
	multi_entry_promise() : state_(promise_state::PREGNANCY) {}

	generator<T> get_return_object() noexcept {
		return generator<T>(multi_entry_handle<T>::from_promise(*this));
	}

	std::suspend_always initial_suspend() noexcept {
		return {};
	}

	std::suspend_never final_suspend() noexcept {
		return {};
	}

	std::coroutine_handle<>& parent_handle() noexcept {
		return parent_handle_;
	}

	void unhandled_exception() {
		// TODO
	}

	std::suspend_always yield_value(T value) {
		if (state_ == promise_state::REPREGNANCY) {
			store_.erase();
		}
		store_.write(std::forward<T>(value));
		transfer_write(state_);
		if (parent_handle() && !parent_handle().done()) {
			scheduler_impl::local()->spawn_handle(parent_handle());
			scheduler_impl::local()->spawn_handle(
				multi_entry_handle<T>::from_promise(*this));
		}
		return {};
	}

	void return_void() {}

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

	~multi_entry_promise() {
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

}  // namespace detail

}  // namespace tiny_coroutine
