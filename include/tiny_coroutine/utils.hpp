#include <coroutine>
#include <ctime>
#include <functional>
#include <optional>

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

template <typename T>
task<std::optional<T>> timeout(task<T> coroutine, time_t sec) {
	auto stop_time = time(nullptr) + sec;
	while (time(nullptr) < stop_time) {
		if (coroutine.await_ready()) co_return co_await coroutine;
		co_await slicer();
	}
	coroutine.abort();
	co_return std::nullopt;
}

}  // namespace tiny_coroutine
