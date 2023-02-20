#pragma once

#include <coroutine>
#include <type_traits>

#include "promis_no_type.hpp"
#include "promise_state.hpp"

namespace tiny_coroutine {

namespace detail {

template <typename PromiseType,
		  typename std::is_base_of<promise_no_type, PromiseType>::type* =
			  nullptr>
class awaiter {
public:
	awaiter(PromiseType* handle) : waited_(handle) {}

	bool await_ready() {
		return waited_->state() == promise_state::Birth;
	}

	template <typename promise>
	void await_suspend(std::coroutine_handle<promise> waiter) {
		waited_->set_listener(&waiter.promise());
	}

	decltype(std::declval<PromiseType>().result())
	await_resume() {  // can not be auto (auto will remove reference)
		return waited_->result();
	}

private:
	PromiseType* waited_;
};

}  // namespace detail

}  // namespace tiny_coroutine
