#include <coroutine>
#include <functional>

#include "task.hpp"
#include "detail/scheduler_impl.hpp"


namespace tiny_coroutine {


struct slicer {
	bool await_ready() {return false;}
	void await_suspend(std::coroutine_handle<> waiter) {
		detail::scheduler_impl::local()->spawn_handle(waiter);
	}
	void await_resume() {}
};


task<void> condition(std::function<bool()> lambda) {
	while (!lambda()) {
		co_await slicer();
	}
	co_return;
}

}
