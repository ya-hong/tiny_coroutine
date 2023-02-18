/**
 * 用于task的promise
 *
 * 可能的状态：
 * - pregnancy
 * - detach_pregnancy
 * - birth
 * - repregnancy
 */

#pragma once

#include <coroutine>

#include "handle.hpp"
#include "promis_no_type.hpp"
#include "promise_state.hpp"
#include "store.hpp"

namespace tiny_coroutine {

namespace detail {

template <typename T>
class promise : public promise_no_type {
public:
	promise() : store_(), promise_no_type(handle<T>::from_promise(*this)) {}

	handle<T> get_return_object() noexcept {
		return handle<T>::from_address(get_handle().address());
	}

	template <typename U>
	void return_value(U&& value) {	// coroutine will be removed from queue
		switch (state_) {
		case promise_state::Pregnancy:
			store_.write(std::forward<U>(value));
			state_ = promise_state::Birth;
			try_schedule_awake_parent();  // Notify the father after the birth
										  // of the child (If there is) XD
			break;

		case promise_state::Detach:
			schedule_awake();  // skip save result, send self to destory
			state_ = promise_state::Abort;
			break;

		default:
			assert(false && "promise state broken");
			break;
		}
	}

	T result() {  // coroutine not in queue
		assert(state_ == promise_state::Birth && "promise state broken");
		state_ = promise_state::RePregnancy;
		return store_.read();
	}

	void detach() {
		switch (state_) {
		case promise_state::Pregnancy:	// coroutine will return queue
			state_ = promise_state::Detach;
			break;
		case promise_state::Detach:
			break;
		default:  // coroutine will not return queue
			abort();
			break;
		}
	}

	void abort() {
		switch (state_) {
		case promise_state::Pregnancy:
		case promise_state::Detach:
			state_ = promise_state::Abort;
			// coroutine will return queue, do not need send to destory
			break;
		case promise_state::Birth:
		case promise_state::RePregnancy:
			store_.erase();
			state_ = promise_state::Abort;
			schedule_awake();  // send to destory
			break;
		}
	}

	~promise() noexcept {
		switch (state_) {
		case promise_state::Birth:
		case promise_state::RePregnancy:
			store_.erase();
			break;
		}
	}

private:
	store<T> store_;
};

template <>
class promise<void> : public promise_no_type {
public:
	promise() : promise_no_type(handle<void>::from_promise(*this)) {}

	handle<void> get_return_object() noexcept {
		return handle<void>::from_promise(*this);
	}

	void return_void() {
		switch (state_) {
		case promise_state::Pregnancy:
			state_ = promise_state::Birth;
			try_schedule_awake_parent();  // Notify the father after the birth
										  // of the child (If there is) XD
			break;

		case promise_state::Detach:
			schedule_awake();  // skip save result, send self to destory
			state_ = promise_state::Abort;
			break;

		default:
			assert(false && "promise state broken");
			break;
		}
	}

	void result() {
		assert(state_ == promise_state::Birth && "promise state broken");
		state_ = promise_state::RePregnancy;
	}

	void detach() {	 // coroutine should be in queue
		switch (state_) {
		case promise_state::Pregnancy:
			state_ = promise_state::Detach;
			break;
		case promise_state::Birth:
			state_ = promise_state::Abort;
			break;
		}
	}

	void abort() {
		state_ = promise_state::Abort;
	}

private:
};

}  // namespace detail

}  // namespace tiny_coroutine
