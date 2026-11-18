#pragma once
#include "../detail/common.h"

namespace classcafe::StackMap
{
	// Verification types
	struct Top               { static constexpr u1 tag = 0; };
	struct Integer           { static constexpr u1 tag = 1; };
	struct Float             { static constexpr u1 tag = 2; };
	struct Double            { static constexpr u1 tag = 3; };
	struct Long              { static constexpr u1 tag = 4; };
	struct Null              { static constexpr u1 tag = 5; };
	struct UninitializedThis { static constexpr u1 tag = 6; };
	struct Object            { static constexpr u1 tag = 7; u2 class_index; };
	struct Uninitialized     { static constexpr u1 tag = 8; u2 new_offset; };

	using VType = std::variant<Top, Integer, Float, Double, Long, Null, 
		UninitializedThis, Object, Uninitialized>;

	// offset_delta is the raw value stored in the class file; 
	// the first frame is at bytecode offset offset_delta, 
	// every later one is at previous_offset + offset_delta + 1
	// The writer picks the short or extended encoding from offset_delta
	struct Same         { u2 offset_delta; };
	struct SameLocals1  { u2 offset_delta; VType stack; };
	struct Chop         { u2 offset_delta; u1 k; };            // k = locals removed, 1..3
	struct Append       { u2 offset_delta; Vec<VType> locals; }; // 1..3 locals
	struct Full         { u2 offset_delta; Vec<VType> locals; Vec<VType> stack; };
	
	using Frame = std::variant<Same, SameLocals1, Chop, Append, Full>;
}