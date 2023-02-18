#pragma once

#include <coroutine>

#include "handle.hpp"
#include "promis_no_type.hpp"
#include "promise_state.hpp"
#include "scheduler_impl.hpp"
#include "store.hpp"

namespace tiny_coroutine {

// template <typename T>
// class generator;

namespace detail {

template <typename T>
class multi_entry_promise : public promise_no_type {
public:
	multi_entry_promise()
		: prev_(),
		  curr_(),
		  promise_no_type(multi_entry_handle<T>::from_promise(*this)) {}

	multi_entry_handle<T> get_return_object() noexcept {
		return multi_entry_handle<T>::from_promise(*this);
	}

	std::suspend_always yield_value(T value) {
		switch (state_) {
		case promise_state::Pregnancy:
			curr_.write(std::forward<T>(value));
			state_ = promise_state::RePregnancy;
			schedule_awake();
			break;
		case promise_state::RePregnancy:
			prev_.write(curr_.read());
			curr_.write(std::forward<T>(value));
			state_ = promise_state::Birth;
			try_schedule_awake_parent();
			break;
		default:
			throw "promise state broken";
			break;
		}

		return {};
	}

	void return_void() {
		switch (state_) {
		case promise_state::Pregnancy:
			break;
		case promise_state::RePregnancy:
			prev_.write(curr_.read());
			state_ = promise_state::Birth;
			try_schedule_awake_parent();
			break;
		default:
			throw "promise state broken";
			break;
		}
	}

	T result() {
		assert(state_ == promise_state::Birth && "promise state no ready");
		state_ = promise_state::RePregnancy;
		if (!get_handle()
				 .done()) {	   // promise will not control life time of self
			schedule_awake();  // deliver child, then reproduction
		}
		return prev_.read();
	}

	void abort() {
		switch (state_) {
		case promise_state::Pregnancy:
			state_ = promise_state::Abort;
			break;
		case promise_state::Birth:
			prev_.erase();
			curr_.erase();
			state_ = promise_state::Abort;
			schedule_awake();  // send to destory
		case promise_state::RePregnancy:
			prev_.erase();
			curr_.erase();
			state_ = promise_state::Abort;
			if (get_handle().done()) schedule_awake();	// send to destory
			break;
		}
	}

	~multi_entry_promise() {
		switch (state_) {
		case promise_state::Birth:
		case promise_state::RePregnancy:
			prev_.erase();
			curr_.erase();
			break;
		}
	}

private:
	store<T> prev_;
	store<T> curr_;
};

}  // namespace detail

}  // namespace tiny_coroutine
