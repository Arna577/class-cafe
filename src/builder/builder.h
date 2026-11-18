#pragma once
#include "../detail/common.h"
#include "../data/access_flags.h"
#include "../data/class_file.h"
#include "../result.h"
#include "type.h"

namespace classcafe
{
	struct ClassImpl;
	struct MethodImpl;

	// Specification for creation

	struct ParameterSpec {
		std::string_view name;
		JVMType type;
		ParameterAccess access;
	};

	// Handles (Internal References)

	class Method {
		friend class Class;

		ClassImpl* cls;
		u4 index;

		Method(ClassImpl* cls, u4 index) : cls(cls), index(index) {}
	public:
		// CodeBuilder code() const;
	};

	class Field {
		friend class Class;

		ClassImpl* cls;
		u4 index;

		Field(ClassImpl* cls, u4 index) : cls(cls), index(index) {}
	public:
		// JVMType get_type() const;
		// void set_type(JVMType ty);
	};

	class Local {
		MethodImpl* mth;
		u4 index;
	public:
	};

	// External References

	struct ExternalClass { std::string name; /* TODO: find more relevant fields */ };
	struct ExternalField { std::string owner, name; JVMType type; };
	struct ExternalMethod { std::string owner, name; Vec<JVMType> params; JVMType returned; bool owner_is_interface = false; };

	// Value

	struct Value {
		struct Const { std::variant<i4, i8, float, double, std::string> v; };
		struct StackTop {};
		std::variant<Const, Local, Field, ExternalField, StackTop> v;
	};

	// TODO: variable handle
	// TODO: implicit conversion of Method/Field/Variable handles to Value

	// Class Builder

	class Class {
		std::unique_ptr<ClassImpl> _impl;
		Class();
	public:
		struct Version { u1 major, minor; };
		struct Superclass {
			std::variant<ExternalClass, ClassImpl*> v;
			std::string_view name() const;
		};

		/// Creation

		static Class make(
			std::string_view name, 
			ClassAccess access = ClassAccess::Default,
			Version version = { 52, 0 }
		);

		/// Building
		
		Method add_method(
			std::string_view name, 
			Slice<ParameterSpec> parameters = {}, 
			JVMType return_type = BaseType::Void, 
			MethodAccess access = MethodAccess::Default
		);

		// TODO: Method addConstructor
		
		Field add_field(
			std::string_view name, 
			JVMType type, 
			FieldAccess access = FieldAccess::Default
		);

		// TODO: addInterface

		/// Properties

		bool is_abstract() const;
		// TODO: std::string descriptor()

		Version get_version() const;
		void set_version(Version v);

		Superclass get_superclass() const;
		void set_superclass(Class c);
		void set_superclass(ExternalClass c);

		/// Finalizing 

		Result<ClassFile> lower() const;

		/// details

		~Class();
		Class(Class&&);
		Class& operator=(Class&&);
	};

	namespace Interface {
		static Class make(
			std::string_view name,
			ClassAccess access = ClassAccess::Default
		) {
			return Class::make(name, access | ClassAccess::Interface | ClassAccess::Abstract);
		}
	}
}