#pragma once
#include "detail/types.h"

namespace classcafe
{
	// Stack effects are in slots, so long and double count as 2.
	// Var means they're derived from a descriptor or operand at emit time
	inline constexpr i1 Var = -1;

	enum class Shape : u1 {
		Simple,         // no operands
		Short,          // slot or constant baked into the opcode; encoder-only
		Int,            // bipush, sipush
		Local, Iinc,
		Ldc,
		Field, Invoke, InvokeDynamic,
		Type, NewArray, MultiANewArray,
		Jump, TableSwitch, LookupSwitch,
		Wide,           // prefix, never emitted directly
	};

	//name            value  mnemonic           shape          pops pushes
#define CLASSCAFE_OPCODES(X) \
	X(Nop,                0, "nop",             Simple,        0, 0) \
	X(AconstNull,         1, "aconst_null",     Simple,        0, 1) \
	X(IconstM1,           2, "iconst_m1",       Simple,        0, 1) \
	X(Iconst0,            3, "iconst_0",        Simple,        0, 1) \
	X(Iconst1,            4, "iconst_1",        Simple,        0, 1) \
	X(Iconst2,            5, "iconst_2",        Simple,        0, 1) \
	X(Iconst3,            6, "iconst_3",        Simple,        0, 1) \
	X(Iconst4,            7, "iconst_4",        Simple,        0, 1) \
	X(Iconst5,            8, "iconst_5",        Simple,        0, 1) \
	X(Lconst0,            9, "lconst_0",        Simple,        0, 2) \
	X(Lconst1,           10, "lconst_1",        Simple,        0, 2) \
	X(Fconst0,           11, "fconst_0",        Simple,        0, 1) \
	X(Fconst1,           12, "fconst_1",        Simple,        0, 1) \
	X(Fconst2,           13, "fconst_2",        Simple,        0, 1) \
	X(Dconst0,           14, "dconst_0",        Simple,        0, 2) \
	X(Dconst1,           15, "dconst_1",        Simple,        0, 2) \
	X(Bipush,            16, "bipush",          Int,           0, 1) \
	X(Sipush,            17, "sipush",          Int,           0, 1) \
	X(Ldc,               18, "ldc",             Ldc,           0, 1) \
	X(LdcW,              19, "ldc_w",           Ldc,           0, 1) \
	X(Ldc2W,             20, "ldc2_w",          Ldc,           0, 2) \
	X(Iload,             21, "iload",           Local,         0, 1) \
	X(Lload,             22, "lload",           Local,         0, 2) \
	X(Fload,             23, "fload",           Local,         0, 1) \
	X(Dload,             24, "dload",           Local,         0, 2) \
	X(Aload,             25, "aload",           Local,         0, 1) \
	X(Iload0,            26, "iload_0",         Short,         0, 1) \
	X(Iload1,            27, "iload_1",         Short,         0, 1) \
	X(Iload2,            28, "iload_2",         Short,         0, 1) \
	X(Iload3,            29, "iload_3",         Short,         0, 1) \
	X(Lload0,            30, "lload_0",         Short,         0, 2) \
	X(Lload1,            31, "lload_1",         Short,         0, 2) \
	X(Lload2,            32, "lload_2",         Short,         0, 2) \
	X(Lload3,            33, "lload_3",         Short,         0, 2) \
	X(Fload0,            34, "fload_0",         Short,         0, 1) \
	X(Fload1,            35, "fload_1",         Short,         0, 1) \
	X(Fload2,            36, "fload_2",         Short,         0, 1) \
	X(Fload3,            37, "fload_3",         Short,         0, 1) \
	X(Dload0,            38, "dload_0",         Short,         0, 2) \
	X(Dload1,            39, "dload_1",         Short,         0, 2) \
	X(Dload2,            40, "dload_2",         Short,         0, 2) \
	X(Dload3,            41, "dload_3",         Short,         0, 2) \
	X(Aload0,            42, "aload_0",         Short,         0, 1) \
	X(Aload1,            43, "aload_1",         Short,         0, 1) \
	X(Aload2,            44, "aload_2",         Short,         0, 1) \
	X(Aload3,            45, "aload_3",         Short,         0, 1) \
	X(Iaload,            46, "iaload",          Simple,        2, 1) \
	X(Laload,            47, "laload",          Simple,        2, 2) \
	X(Faload,            48, "faload",          Simple,        2, 1) \
	X(Daload,            49, "daload",          Simple,        2, 2) \
	X(Aaload,            50, "aaload",          Simple,        2, 1) \
	X(Baload,            51, "baload",          Simple,        2, 1) \
	X(Caload,            52, "caload",          Simple,        2, 1) \
	X(Saload,            53, "saload",          Simple,        2, 1) \
	X(Istore,            54, "istore",          Local,         1, 0) \
	X(Lstore,            55, "lstore",          Local,         2, 0) \
	X(Fstore,            56, "fstore",          Local,         1, 0) \
	X(Dstore,            57, "dstore",          Local,         2, 0) \
	X(Astore,            58, "astore",          Local,         1, 0) \
	X(Istore0,           59, "istore_0",        Short,         1, 0) \
	X(Istore1,           60, "istore_1",        Short,         1, 0) \
	X(Istore2,           61, "istore_2",        Short,         1, 0) \
	X(Istore3,           62, "istore_3",        Short,         1, 0) \
	X(Lstore0,           63, "lstore_0",        Short,         2, 0) \
	X(Lstore1,           64, "lstore_1",        Short,         2, 0) \
	X(Lstore2,           65, "lstore_2",        Short,         2, 0) \
	X(Lstore3,           66, "lstore_3",        Short,         2, 0) \
	X(Fstore0,           67, "fstore_0",        Short,         1, 0) \
	X(Fstore1,           68, "fstore_1",        Short,         1, 0) \
	X(Fstore2,           69, "fstore_2",        Short,         1, 0) \
	X(Fstore3,           70, "fstore_3",        Short,         1, 0) \
	X(Dstore0,           71, "dstore_0",        Short,         2, 0) \
	X(Dstore1,           72, "dstore_1",        Short,         2, 0) \
	X(Dstore2,           73, "dstore_2",        Short,         2, 0) \
	X(Dstore3,           74, "dstore_3",        Short,         2, 0) \
	X(Astore0,           75, "astore_0",        Short,         1, 0) \
	X(Astore1,           76, "astore_1",        Short,         1, 0) \
	X(Astore2,           77, "astore_2",        Short,         1, 0) \
	X(Astore3,           78, "astore_3",        Short,         1, 0) \
	X(Iastore,           79, "iastore",         Simple,        3, 0) \
	X(Lastore,           80, "lastore",         Simple,        4, 0) \
	X(Fastore,           81, "fastore",         Simple,        3, 0) \
	X(Dastore,           82, "dastore",         Simple,        4, 0) \
	X(Aastore,           83, "aastore",         Simple,        3, 0) \
	X(Bastore,           84, "bastore",         Simple,        3, 0) \
	X(Castore,           85, "castore",         Simple,        3, 0) \
	X(Sastore,           86, "sastore",         Simple,        3, 0) \
	X(Pop,               87, "pop",             Simple,        1, 0) \
	X(Pop2,              88, "pop2",            Simple,        2, 0) \
	X(Dup,               89, "dup",             Simple,        1, 2) \
	X(DupX1,             90, "dup_x1",          Simple,        2, 3) \
	X(DupX2,             91, "dup_x2",          Simple,        3, 4) \
	X(Dup2,              92, "dup2",            Simple,        2, 4) \
	X(Dup2X1,            93, "dup2_x1",         Simple,        3, 5) \
	X(Dup2X2,            94, "dup2_x2",         Simple,        4, 6) \
	X(Swap,              95, "swap",            Simple,        2, 2) \
	X(Iadd,              96, "iadd",            Simple,        2, 1) \
	X(Ladd,              97, "ladd",            Simple,        4, 2) \
	X(Fadd,              98, "fadd",            Simple,        2, 1) \
	X(Dadd,              99, "dadd",            Simple,        4, 2) \
	X(Isub,             100, "isub",            Simple,        2, 1) \
	X(Lsub,             101, "lsub",            Simple,        4, 2) \
	X(Fsub,             102, "fsub",            Simple,        2, 1) \
	X(Dsub,             103, "dsub",            Simple,        4, 2) \
	X(Imul,             104, "imul",            Simple,        2, 1) \
	X(Lmul,             105, "lmul",            Simple,        4, 2) \
	X(Fmul,             106, "fmul",            Simple,        2, 1) \
	X(Dmul,             107, "dmul",            Simple,        4, 2) \
	X(Idiv,             108, "idiv",            Simple,        2, 1) \
	X(Ldiv,             109, "ldiv",            Simple,        4, 2) \
	X(Fdiv,             110, "fdiv",            Simple,        2, 1) \
	X(Ddiv,             111, "ddiv",            Simple,        4, 2) \
	X(Irem,             112, "irem",            Simple,        2, 1) \
	X(Lrem,             113, "lrem",            Simple,        4, 2) \
	X(Frem,             114, "frem",            Simple,        2, 1) \
	X(Drem,             115, "drem",            Simple,        4, 2) \
	X(Ineg,             116, "ineg",            Simple,        1, 1) \
	X(Lneg,             117, "lneg",            Simple,        2, 2) \
	X(Fneg,             118, "fneg",            Simple,        1, 1) \
	X(Dneg,             119, "dneg",            Simple,        2, 2) \
	X(Ishl,             120, "ishl",            Simple,        2, 1) \
	X(Lshl,             121, "lshl",            Simple,        3, 2) \
	X(Ishr,             122, "ishr",            Simple,        2, 1) \
	X(Lshr,             123, "lshr",            Simple,        3, 2) \
	X(Iushr,            124, "iushr",           Simple,        2, 1) \
	X(Lushr,            125, "lushr",           Simple,        3, 2) \
	X(Iand,             126, "iand",            Simple,        2, 1) \
	X(Land,             127, "land",            Simple,        4, 2) \
	X(Ior,              128, "ior",             Simple,        2, 1) \
	X(Lor,              129, "lor",             Simple,        4, 2) \
	X(Ixor,             130, "ixor",            Simple,        2, 1) \
	X(Lxor,             131, "lxor",            Simple,        4, 2) \
	X(Iinc,             132, "iinc",            Iinc,          0, 0) \
	X(I2l,              133, "i2l",             Simple,        1, 2) \
	X(I2f,              134, "i2f",             Simple,        1, 1) \
	X(I2d,              135, "i2d",             Simple,        1, 2) \
	X(L2i,              136, "l2i",             Simple,        2, 1) \
	X(L2f,              137, "l2f",             Simple,        2, 1) \
	X(L2d,              138, "l2d",             Simple,        2, 2) \
	X(F2i,              139, "f2i",             Simple,        1, 1) \
	X(F2l,              140, "f2l",             Simple,        1, 2) \
	X(F2d,              141, "f2d",             Simple,        1, 2) \
	X(D2i,              142, "d2i",             Simple,        2, 1) \
	X(D2l,              143, "d2l",             Simple,        2, 2) \
	X(D2f,              144, "d2f",             Simple,        2, 1) \
	X(I2b,              145, "i2b",             Simple,        1, 1) \
	X(I2c,              146, "i2c",             Simple,        1, 1) \
	X(I2s,              147, "i2s",             Simple,        1, 1) \
	X(Lcmp,             148, "lcmp",            Simple,        4, 1) \
	X(Fcmpl,            149, "fcmpl",           Simple,        2, 1) \
	X(Fcmpg,            150, "fcmpg",           Simple,        2, 1) \
	X(Dcmpl,            151, "dcmpl",           Simple,        4, 1) \
	X(Dcmpg,            152, "dcmpg",           Simple,        4, 1) \
	X(Ifeq,             153, "ifeq",            Jump,          1, 0) \
	X(Ifne,             154, "ifne",            Jump,          1, 0) \
	X(Iflt,             155, "iflt",            Jump,          1, 0) \
	X(Ifge,             156, "ifge",            Jump,          1, 0) \
	X(Ifgt,             157, "ifgt",            Jump,          1, 0) \
	X(Ifle,             158, "ifle",            Jump,          1, 0) \
	X(IfIcmpeq,         159, "if_icmpeq",       Jump,          2, 0) \
	X(IfIcmpne,         160, "if_icmpne",       Jump,          2, 0) \
	X(IfIcmplt,         161, "if_icmplt",       Jump,          2, 0) \
	X(IfIcmpge,         162, "if_icmpge",       Jump,          2, 0) \
	X(IfIcmpgt,         163, "if_icmpgt",       Jump,          2, 0) \
	X(IfIcmple,         164, "if_icmple",       Jump,          2, 0) \
	X(IfAcmpeq,         165, "if_acmpeq",       Jump,          2, 0) \
	X(IfAcmpne,         166, "if_acmpne",       Jump,          2, 0) \
	X(Goto,             167, "goto",            Jump,          0, 0) \
	X(Jsr,              168, "jsr",             Jump,          0, 1) \
	X(Ret,              169, "ret",             Local,         0, 0) \
	X(Tableswitch,      170, "tableswitch",     TableSwitch,   1, 0) \
	X(Lookupswitch,     171, "lookupswitch",    LookupSwitch,  1, 0) \
	X(Ireturn,          172, "ireturn",         Simple,        1, 0) \
	X(Lreturn,          173, "lreturn",         Simple,        2, 0) \
	X(Freturn,          174, "freturn",         Simple,        1, 0) \
	X(Dreturn,          175, "dreturn",         Simple,        2, 0) \
	X(Areturn,          176, "areturn",         Simple,        1, 0) \
	X(Return,           177, "return",          Simple,        0, 0) \
	X(Getstatic,        178, "getstatic",       Field,         0, Var) \
	X(Putstatic,        179, "putstatic",       Field,         Var, 0) \
	X(Getfield,         180, "getfield",        Field,         1, Var) \
	X(Putfield,         181, "putfield",        Field,         Var, 0) \
	X(Invokevirtual,    182, "invokevirtual",   Invoke,        Var, Var) \
	X(Invokespecial,    183, "invokespecial",   Invoke,        Var, Var) \
	X(Invokestatic,     184, "invokestatic",    Invoke,        Var, Var) \
	X(Invokeinterface,  185, "invokeinterface", Invoke,        Var, Var) \
	X(Invokedynamic,    186, "invokedynamic",   InvokeDynamic, Var, Var) \
	X(New,              187, "new",             Type,          0, 1) \
	X(Newarray,         188, "newarray",        NewArray,      1, 1) \
	X(Anewarray,        189, "anewarray",       Type,          1, 1) \
	X(Arraylength,      190, "arraylength",     Simple,        1, 1) \
	X(Athrow,           191, "athrow",          Simple,        1, 0) \
	X(Checkcast,        192, "checkcast",       Type,          1, 1) \
	X(Instanceof,       193, "instanceof",      Type,          1, 1) \
	X(Monitorenter,     194, "monitorenter",    Simple,        1, 0) \
	X(Monitorexit,      195, "monitorexit",     Simple,        1, 0) \
	X(Wide,             196, "wide",            Wide,          0, 0) \
	X(Multianewarray,   197, "multianewarray",  MultiANewArray, Var, 1) \
	X(Ifnull,           198, "ifnull",          Jump,          1, 0) \
	X(Ifnonnull,        199, "ifnonnull",       Jump,          1, 0) \
	X(GotoW,            200, "goto_w",          Jump,          0, 0) \
	X(JsrW,             201, "jsr_w",           Jump,          0, 1)

	enum class Opcode : u1 {
#define X(name, value, mnemonic, shape, pops, pushes) name = value,
		CLASSCAFE_OPCODES(X)
#undef X
	};

	struct OpcodeInfo {
		char const* mnemonic;
		Shape shape;
		i1 pops, pushes;
	};

	inline constexpr OpcodeInfo opcode_table[] = {
		#define X(name, value, mnemonic, shape, pops, pushes) \
			{ mnemonic, Shape::shape, pops, pushes },
		CLASSCAFE_OPCODES(X)
		#undef X
	};

	inline constexpr int opcode_count = sizeof(opcode_table) / sizeof(opcode_table[0]);
	static_assert(opcode_count == 202, "opcodes 0..201 must all be present");

	// Row i must hold opcode value i, so the table can be indexed directly.
	inline constexpr u1 opcode_values[] = {
		#define X(name, value, mnemonic, shape, pops, pushes) value,
		CLASSCAFE_OPCODES(X)
		#undef X
	};
	constexpr bool table_is_ordered() {
		for (int i = 0; i < opcode_count; ++i)
			if (opcode_values[i] != i) return false;
		return true;
	}
	static_assert(table_is_ordered(), "rows must be in opcode order, with no gaps or duplicates");

	inline const OpcodeInfo& info(Opcode op) { return opcode_table[static_cast<u1>(op)]; }
}