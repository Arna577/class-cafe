#pragma once
#include <variant>
#include "errors.h"

namespace classcafe
{
	template <class T>
	class Result {
		std::variant<T, Error> v;
	public:
		Result(T val) : v(std::move(val)) {}
		Result(Error e) : v(e) {}
		explicit operator bool() const { return v.index() == 0; }
		T& operator*() { return std::get<0>(v); }
		Error error() const { return std::get<1>(v); }
	};
}