#pragma once

namespace classcafe {
	enum class Error {
		None = 0,

		InvalidUTF8,
		StringTooLong,

		// Emission
		TooManyItems,        // a count that is stored as u1/u2 does not fit
		TooLong,             // an attribute body is longer than a u4 length can describe
		BadConstantPool,     // missing slot 0, or Long/Double without its Unusable slot, or stray Unusable
		BadCode,             // Code attribute with empty code or code_length > 65535
		BadStackMapFrame,    // Chop k outside 1..3, Append with 0 or more than 3 locals
	};
}