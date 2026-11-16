#pragma once
#include "detail/common.h"
#include "opcodes.h"
#include <variant>

namespace classcafe
{
	struct Label { u4 id; };
	struct Str { std::string text; };
	struct ClassName { std::string internal_name; };
	using LdcValue = std::variant<i4, float, i8, double, Str, ClassName>;

	namespace Insn {
		struct Simple { Opcode op; };
		struct Int { Opcode op; i4 value; };   // bipush, sipush, iconst_n, or ldc, by value
		struct Local { Opcode op; u2 slot; };
		struct Iinc { u2 slot; i2 delta; };
		struct Ldc { LdcValue value; };        // encoder picks ldc / ldc_w / ldc2_w
		// struct FieldAccess { Opcode op; Field ref; };
		// struct Call { Opcode op; Method ref; }; // invokeinterface's count is derived
		// struct NewArray { PrimKind element; };
		struct TypeOp { Opcode op; std::string cls; };
		struct MultiNew { std::string descriptor; u1 dims; };
		struct Jump { Opcode op; Label target; };
		struct TableSwitch { i4 low; Label dflt; Vec<Label> targets; };
		struct LookupSwitch { Label dflt; Vec<std::pair<i4, Label>> pairs; };
		struct Mark { Label label; };           // pseudo-instruction, size 0
	}

	using Instruction = std::variant<Insn::Simple, Insn::Int, Insn::Local, Insn::Iinc, Insn::Ldc,
		// Insn::FieldAccess, Insn::Call, Insn::NewArray, 
		Insn::TypeOp, Insn::MultiNew,
		Insn::Jump, Insn::TableSwitch, Insn::LookupSwitch, Insn::Mark>;
}