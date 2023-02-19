#pragma once

#include "detail/awaiter.hpp"
#include "detail/handle.hpp"
#include "detail/promise_state.hpp"
#include "detail/scheduler_impl.hpp"
#include "detail/task_promise.hpp"

namespace tiny_coroutine {

template <typename T, typename life_time_strategy>
class task;

namespace strategy {

struct detach {	 // coroutine lifetime and task lifetime are detached
	template <typename T>
	static void deconstruct(task<T, detach>* task) {
		if (task->pr_handle_) {
			task->pr_handle_->detach();
		}
	}

	template <typename T, typename S>
	static task<T, detach> convert(task<T, S>&& t) {
		return task<T, detach>{std::move(t)};
	}
};

struct attach {	 // coroutine lifetime and task lifetime are attached
	template <typename T>
	static void deconstruct(task<T, attach>* task) {
		if (task->pr_handle_) {
			task->pr_handle_->abort();
		}
	}

	template <typename T, typename S>
	static task<T, attach> convert(task<T, S>&& t) {
		return task<T, attach>{std::move(t)};
	}
};

}  // namespace strategy

template <typename T, typename life_time_strategy = strategy::attach>
class task {
	friend class strategy::detach;
	friend class strategy::attach;

	template <typename U, typename S>
	friend class task;

public:
	using promise_type = detail::promise<T>;
	using awaiter = detail::awaiter<promise_type>;

	task() = delete;

	task(const task&) = delete;

	template <typename S>
	explicit task(task<T, S>&& other) : pr_handle_(other.pr_handle_) {
		other.pr_handle_ = nullptr;
	}

	task& operator=(const task&) = delete;

	template <typename S>
	task& operator=(task<T, S>&& other) {
		std::swap(pr_handle_, other.pr_handle_);
	}

	task(promise_type* h) : pr_handle_(h) {
		pr_handle_->schedule_awake();
	}

	awaiter operator co_await() const {
		return awaiter{pr_handle_};
	}

	bool done() const {	 // this task<> manage a coroutine and coroutine done
		return pr_handle_ && pr_handle_->get_co_handle().done();
	}

	void abort() {
		if (pr_handle_ == nullptr) [[unlikely]] {
			throw "this task<> no longer manages the lifetime of coroutines";
		}
		pr_handle_->abort();
		pr_handle_ = nullptr;
	}

	void detach() {
		if (pr_handle_ == nullptr) [[unlikely]] {
			throw "this task<> no longer manages the lifetime of coroutines";
		}
		pr_handle_->detach();
		pr_handle_ = nullptr;
	}

	~task() {
		life_time_strategy::deconstruct(this);
	}

private:
	promise_type* pr_handle_;
};

}  // namespace tiny_coroutine
