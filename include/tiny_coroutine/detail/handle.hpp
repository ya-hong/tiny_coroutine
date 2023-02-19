#pragma once

#include <coroutine>

namespace tiny_coroutine {

namespace detail {

class promise_no_type;

template <typename T>
class promise;

template <typename T>
class multi_entry_promise;

using co_handle = std::coroutine_handle<>;

using pr_handle = promise_no_type*;

template <typename T>
using multi_entry_handle = std::coroutine_handle<multi_entry_promise<T>>;

}  // namespace detail

}  // namespace tiny_coroutine
