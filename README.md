# tiny_coroutine
基于C++20的单线程并发协程框架

[参考 Codesire-Deng/co_context](https://github.com/Codesire-Deng/co_context)


## 实现思路

所有协程都由调度器发起，协程suspend后控制权回到调度器。调度器内通过一个队列进行时间片轮转实现并发（时间片为协程从上一个暂停点到下一个暂停点的片段）。


## 特点

**优点**

1. 单线程并发，不需要锁
2. 没有传染性，不会把非协程函数也变成协程
3. 只使用标准库。没有使用第三方库，也没有使用系统调用。

**缺点**

1. 时间片轮转实现比较暴力。协程切换之间有大量的跳转、调用，会影响性能。
2. 是无栈协程，且所有协程的调用方都是调度器，调试比较困难。
3. 没有封装任何协程io操作，所以如果要非阻塞需要在另外的线程执行io T\_T


## example

task

```cpp
#include <iostream>
#include <string>
#include "tiny_coroutine.hpp"
using namespace tiny_coroutine;

task<void>enter() {
	std::cout << "enter coroutine" << std::endl;
	co_return;
}

task<void> hello() {
	co_await timeout(2);
	std::cout << "hello coroutine" << std::endl;
	co_return;
}

int main() {
	scheduler s;
	s.spawn(enter);
	s.spawn(hello);
	s.join();
	return 0;
}
```

超时控制

```cpp
task<int>hard_work() {
	co_await timeout(5);
	co_return 114514;
}

task<int> simple_work() {
	co_await timeout(2);
	co_return 19260817;
}

task<void> task1() {
	std::optional<int> result = co_await timeout(hard_work(), 3);
	if (!result) {
		std::cout << "task1: hard work time out" << std::endl;
	}
	else {
		std::cout << "task1: hard word result " << *result << std::endl;
	}
}

task<void> task2() {
	auto w1 = hard_work();
	auto w2 = simple_work();
	co_await timeout(when_all(w1, w2), 3);
	if (w1.done()) {
		std::cout << "task2: hard work result " << co_await w1 << std::endl;
	}
	if (w2.done()) {
		std::cout << "task2: simple work result " << co_await w2 << std::endl;
	}
}
```

generator

```cpp
generator<int> gen(int x) {
	while (true) {
		co_yield x++;
	}
}

task<void> print() {
	auto g = gen(5);
	for (int i = 0; i < 3 && !g.done(); ++i) {
		std::cout << co_await g << std::endl;
	}
	co_return;
}
```

## 主要组件


### scheduler

协程调度器。一个调度器绑定一个线程（使用多个调度器可以在多个线程上运行协程）。

#### `scheduler::spawn(task_function, args...)`

调用一个协程。

#### `scheduler::join()`

阻塞等待到所有调度器上的所有协程运行结束。

### `task<ReturnType, LifeTimeStrategy>`

task是协程函数的返回值。task类型的协程只能通过`co_return`返回一次 / `co_await`获取返回值一次。

可以移动赋值、移动构造，不可拷贝赋值、拷贝构造。

模板参数：

- **ReturnType** `co_return expr` 的类型
- **LifeTimeStrategy** 管理协程生命周期的策略。可取值为`attach | detach`

#### 协程生命周期

task有两种管理协程生命周期的模式:

- **attach** 当task析构时，调度器添加销毁协程的任务
- **detach** 当task析构时，将协程标记为detach。调度器会继续运行协程直到销毁。

当task被销毁前，可以通过移动语义来改变管理协程生命周期的模式。


### `generator<ReturnType>`

generator是生成器协程函数的返回值。generator可以多次`co_yield`，返回多个结果；并最后执行`co_return`结束协程。

可以移动赋值、移动构造，不可拷贝赋值、拷贝构造。


模板参数：

- **ReturnType** `co_yield expr` 的类型


#### 协程生命周期

当generator析构时，调度器添加销毁协程的任务
