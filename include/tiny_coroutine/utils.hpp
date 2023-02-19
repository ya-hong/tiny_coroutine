#include <coroutine>
#include <ctime>
#include <functional>
#include <optional>
#include <vector>

#include "detail/promis_no_type.hpp"
#include "detail/scheduler_impl.hpp"
#include "task.hpp"

namespace tiny_coroutine {

struct slicer {
	bool await_ready() {
		return false;
	}

	template <typename promise>
	void await_suspend(std::coroutine_handle<promise> waiter) {
		waiter.promise().schedule_awake();
	}
	void await_resume() {}
};

task<void> condition(std::function<bool()> lambda) {
	while (!lambda()) {
		co_await slicer();
	}
	co_return;
}

task<void> sleep(time_t sec) {
	auto stop_time = time(nullptr) + sec;
	while (time(nullptr) < stop_time) {
		co_await slicer();
	}
	co_return;
}

template <typename T, typename S>
task<std::optional<T>> timeout(task<T, S>& coroutine, time_t sec) {
	auto stop_time = time(nullptr) + sec;
	while (time(nullptr) < stop_time) {
		if (coroutine.done()) co_return co_await coroutine;
		co_await slicer();
	}
	co_return std::nullopt;
}

template <typename T, typename S>
task<std::optional<T>> timeout(task<T, S>&& coroutine, time_t sec) {
	auto stop_time = time(nullptr) + sec;
	while (time(nullptr) < stop_time) {
		if (coroutine.done()) co_return co_await coroutine;
		co_await slicer();
	}
	co_return std::nullopt;
}

template <typename T, typename S>
task<void, strategy::attach> void_task(task<T, S>&& coroutine) {
	co_await coroutine;
	co_return;
}

template <typename... TaskType>
task<void> when_any(TaskType&&... tasks) {
	bool any = false;
	while (!any) {
		for (auto&& done : {tasks.done()...}) {
			if (done) {
				any = true;
				break;
			}
		}
		co_await slicer();
	}
	co_return;
}

template <typename... TaskType>
task<void> when_all(TaskType&&... tasks) {
	bool all = false;
	while (!all) {
		all = true;
		for (auto&& done : {tasks.done()...}) {
			all &= done;
		}
		co_await slicer();
	}
	co_return;
}

}  // namespace tiny_coroutine
