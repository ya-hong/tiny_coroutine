#include <coroutine>
#include <functional>

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

}  // namespace tiny_coroutine
