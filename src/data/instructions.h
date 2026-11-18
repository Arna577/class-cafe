#pragma once
#include "../detail/common.h"
#include "../builder/type.h"
#include "../builder/builder.h"
#include "../result.h"
#include "opcodes.h"

namespace classcafe
{
	struct Label { u4 id; };
	struct Str { std::string text; };
	struct ClassName { std::string internal_name; };
	using LdcValue = std::variant<i4, float, i8, double, Str, ClassName>;

	enum class BinKind : u1 {
		// The JVM has no operation that compares two values and produces one, they all branch
		Add, Sub, Mul, Div, Rem,
		And, Or, Xor,
		Shl, Shr, UShr,
	};

	namespace Insn {
		// Mid-level (closely represents a family of operations)

		struct Simple { Opcode op; };
		struct Int { Opcode op; i4 value; };   // bipush, sipush, iconst_n, or ldc, by value
		struct Local { Opcode op; u2 slot; };
		struct Iinc { u2 slot; i2 delta; };
		struct Ldc { LdcValue value; };        // encoder picks ldc / ldc_w / ldc2_w
		struct FieldAccess { Opcode op; std::variant<Field, ExternalField> field; };
		struct Call { Opcode op; std::variant<Method, ExternalMethod> method; }; // invokeinterface's count is derived
		struct NewArray { BaseType element; };
		struct TypeOp { Opcode op; std::string cls; };
		struct MultiNew { std::string descriptor; u1 dims; };
		struct Jump { Opcode op; Label target; };
		struct TableSwitch { i4 low; Label dflt; Vec<Label> targets; };
		struct LookupSwitch { Label dflt; Vec<std::pair<i4, Label>> pairs; };

		// High-level (has to be lowered)

		struct BinOp { u2 kind; Value lhs, rhs; };
		struct Load { Value value; };
		struct Store { classcafe::Local dst; Value src; };
		struct Mark { Label label; };           // pseudo-instruction, size 0
	}

	using Instruction = std::variant<Insn::Simple, Insn::Int, Insn::Local, Insn::Iinc, Insn::Ldc,
		Insn::FieldAccess, Insn::Call, Insn::NewArray, 
		Insn::TypeOp, Insn::MultiNew,
		Insn::Jump, Insn::TableSwitch, Insn::LookupSwitch,
		
		Insn::BinOp, Insn::Load, Insn::Store, Insn::Mark>;

	bool newarray_type_ok(BaseType t) { return t >= BaseType::Boolean && t <= BaseType::Long; }
}