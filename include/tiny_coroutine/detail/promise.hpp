#pragma once

#include <coroutine>

#include "handle.hpp"
#include "scheduler_impl.hpp"
// #include "../task.hpp"

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

	std::suspend_always final_suspend() noexcept {
		if (parent_handle() && !parent_handle().done()) {
			std::cout << parent_handle().address() << std::endl;
			scheduler_impl::local()->spawn_handle(parent_handle());
		}
		return {};
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
class temporary_store {
public:
	void return_value(T&& value) {
		std::construct_at(&value_, std::forward<T>(value));
	}

	T result() {
		return std::forward<T>(value_);
	}

private:
	T value_;
};

template <typename T>
class temporary_store<T&> {
public:
	void return_value(T& value) {
		ptr_ = &value;
	}

	T& result() {
		return *ptr_;
	}

private:
	T* ptr_;
};

template <>
class temporary_store<void> {
public:
	void return_void() {}
	void result() {}
};

template <typename T>
class promise : public promise_no_type, public temporary_store<T> {
public:
	task<T> get_return_object() noexcept {
		return task<T>(handle<T>::from_promise(*this));
	}
};

}  // namespace detail

}  // namespace tiny_coroutine