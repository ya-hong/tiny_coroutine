#pragma once

#include <coroutine>


namespace tiny_coroutine {

namespace detail {

template <typename T>
class promise;

template <typename T>
class multi_entry_promise;

template <typename T>
using handle = std::coroutine_handle<promise<T>>;

template <typename T>
using multi_entry_handle = std::coroutine_handle<multi_entry_promise<T>>;

}

}