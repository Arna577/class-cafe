#pragma once
#include "detail/common.h"
#include <variant>

namespace classcafe::Annotations
{
	enum class ConstKind : u1 {
		Byte = 'B',
		Char = 'C',
		Int = 'I',
		Short = 'S',
		Boolean = 'Z',
		Double = 'D',
		Float = 'F',
		Long = 'J',
		String = 's',
	};

	struct Const {
		ConstKind kind;
		u2 const_value_index;
	};

	struct EnumConst {
		u2 type_name_index;
		u2 const_name_index;
	};

	struct ClassInfo {
		u2 class_info_index;
	};

	struct ElementValue;
	struct ElementValuePair;

	struct Annotation {
		u2 type_index;
		Vec<ElementValuePair> pairs;
	};

	struct ArrayValue {
		Vec<ElementValue> values;
	};

	struct ElementValue {
		std::variant<Const, EnumConst, ClassInfo, Annotation, ArrayValue> value;
	};

	struct ElementValuePair {
		u2 name_index;
		ElementValue value;
	};
}