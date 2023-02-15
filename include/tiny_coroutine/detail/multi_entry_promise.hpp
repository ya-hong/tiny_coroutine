#pragma once

#include <coroutine>

#include "handle.hpp"
#include "scheduler_impl.hpp"


namespace tiny_coroutine {

template <typename T>
class generator;

namespace detail {

template <typename T>
class multi_temporary_store {
public:
	multi_temporary_store() : stored_(false) {}

	T result() {
		stored_ = false;
		return std::forward<T>(value_);
	}

	bool has_result() {
		return stored_;
	}

protected:
	void update_value(T&& value) {
		std::construct_at(&value_, std::forward<T>(value));
		stored_ = true;
	}

private:
	T value_;
	bool stored_;
};

template <typename T>
class multi_temporary_store<T&> {
public:
	multi_temporary_store() : stored_(false) {}

	T& result() {
		stored_ = false;
		return *ptr_;
	}

	bool has_result() {
		return stored_;
	}

protected:
	void update_value(T& value) {
		ptr_ = &value;
		stored_ = true;
	}

private:
	T* ptr_;
	bool stored_;
};


template <typename T>
class multi_entry_promise : public multi_temporary_store<T> {
public:
	generator<T> get_return_object() noexcept {
		return generator<T>(multi_entry_handle<T>::from_promise(*this));
	}

	std::suspend_always initial_suspend() noexcept {return {};}

	std::suspend_never final_suspend() noexcept {return {};}

	std::coroutine_handle<>& parent_handle() noexcept {
		return parent_handle_;
	}

	void unhandled_exception() {
		// TODO
	}

	std::suspend_always yield_value(T value) {
		std::cout << "yield value " << value << std::endl;
		multi_temporary_store<T>::update_value(std::forward<T>(value));
		if (parent_handle() && !parent_handle().done()) {
			std::cout << "spawn parent" << std::endl;
			scheduler_impl::local()->spawn_handle(parent_handle());
			scheduler_impl::local()->spawn_handle(multi_entry_handle<T>::from_promise(*this));
		}
		return {};
	}

	void return_void() {}

private:
	std::coroutine_handle<> parent_handle_;
};


}

}