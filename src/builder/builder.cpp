#include "builder.h"

namespace classcafe
{
	// Uninterned, mutable representations

	struct ParameterBody {
		std::string name;
		ParameterAccess access;
		JVMType type;

		ParameterBody(ParameterSpec const& spec)
			: name(spec.name), access(spec.access), type(spec.type) {
		}
	};

	struct MethodBody {
		std::string name;
		MethodAccess access;
		std::vector<ParameterBody> parameters;
		JVMType return_type;

		// InsnList insns;
		// Vec<LocalInfo> locals;    // slot, Type, optional name, optional label range
		// Vec<TryCatch> handlers;   // label-based
		// Vec<LineEntry> lines;
	};

	struct FieldBody {
		std::string name;
		FieldAccess access;
		JVMType type;
		// perhaps an initializer?
	};

	// Class Builder

	struct ClassImpl {
		std::string name;
		ClassAccess access;
		Vec<MethodBody> methods;
		Vec<FieldBody> fields;
	};

	Class Class::make(std::string_view name, ClassAccess access)
	{
		Class b;
		b._impl = std::make_unique<ClassImpl>(std::string(name), access);
		return b;
	}

	Method Class::add_method(std::string_view name, Slice<ParameterSpec> parameters, JVMType return_type, MethodAccess access)
	{
		auto id = _impl->methods.size();

		_impl->methods.emplace_back(
			std::string(name), 
			access,
			Vec<ParameterBody>(parameters.begin(), parameters.end()),
			return_type
		);

		return Method(_impl.get(), u4(id));
	}

	Field Class::add_field(std::string_view name, JVMType type, FieldAccess access)
	{
		auto id = _impl->fields.size();

		_impl->fields.emplace_back(
			std::string(name),
			access,
			type
		);

		return Field(_impl.get(), u4(id));
	}

	Class::Class() = default;
	Class::~Class() = default;
	Class::Class(Class&&) = default;
	Class& Class::operator=(Class&&) = default;

	// 
}