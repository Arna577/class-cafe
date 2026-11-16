#pragma once

namespace classcafe {
	enum class Error {
		None = 0,

		InvalidUTF8,
		StringTooLong,
	};
}