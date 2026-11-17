#pragma once
#include "../detail/common.h"
#include "../access_flags.h"
#include "type.h"

namespace classcafe
{
	struct ClassImpl;

	// Specification for creation

	struct ParameterSpec {
		std::string_view name;
		JVMType type;
		ParameterAccess access;
	};

	// Handles

	class Method {
		friend class Class;

		ClassImpl* cls;
		u4 index;

		Method(ClassImpl* cls, u4 index) : cls(cls), index(index) {}
	public:
	};

	class Field {
		friend class Class;

		ClassImpl* cls;
		u4 index;

		Field(ClassImpl* cls, u4 index) : cls(cls), index(index) {}
	public:
	};

	// TODO: variable handle
	// TODO: struct Value
	// TODO: implicit conversion of Method/Field/Variable handles to Value

	// Class Builder

	class Class {
		std::unique_ptr<ClassImpl> _impl;
		Class();
	public:
		/// Creation

		static Class make(
			std::string_view name, 
			ClassAccess access = ClassAccess::Default
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

		// TODO: std::string descriptor()

		/// details

		~Class();
		Class(Class&&);
		Class& operator=(Class&&);
	};
}