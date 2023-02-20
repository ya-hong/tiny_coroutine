#pragma once

#include <memory>

/*

temporary variable read & write.
avoid manage life time of temporary directly variables in promise

*/

namespace tiny_coroutine {

namespace detail {

template <typename T>
struct store {
	template <typename U>
	void write(U&& value) {
		std::construct_at(&store_.value_, std::move(value));
	}

	T read() {
		return std::move(store_.value_);
	}

	void erase() {
		store_.value_.~T();
	}

private:
	union store_type {
		T value_;
		store_type() {}
	} store_;
};

template <typename T>
struct store<T&> {
	void write(T& value) {
		ptr_ = &value;
	}
	T& read() {
		return *ptr_;
	}
	void erase() {}

private:
	T* ptr_;
};

template <>
struct store<void> {
	void write() {}
	void read() {}

private:
};

}  // namespace detail

}  // namespace tiny_coroutine
