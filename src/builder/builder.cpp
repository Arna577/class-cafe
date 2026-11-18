#include "builder.h"
#include "../data/constant_pool.h"

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

	struct MethodImpl {
		std::string name;
		MethodAccess access;
		Vec<ParameterBody> parameters;
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
		Class::Version version;
		Class::Superclass superclass;
		Vec<std::unique_ptr<MethodImpl>> methods;
		Vec<FieldBody> fields;
	};

	std::string_view Class::Superclass::name() const
	{
		if (auto* ext = std::get_if<ExternalClass>(&v))
			return ext->name;

		return std::get<ClassImpl*>(v)->name;
	}

	/// Creation

	Class Class::make(std::string_view name, ClassAccess access, Version version)
	{
		Class b;
		b._impl = std::make_unique<ClassImpl>(
			std::string(name), 
			access | ClassAccess::Super, 
			version, 
			Class::Superclass(ExternalClass("java/lang/Object"))
		);
		return b;
	}

	/// Building

	Method Class::add_method(std::string_view name, Slice<ParameterSpec> parameters, JVMType return_type, MethodAccess access)
	{
		auto id = _impl->methods.size();

		_impl->methods.emplace_back(std::make_unique<MethodImpl>(
			std::string(name), 
			access,
			Vec<ParameterBody>(parameters.begin(), parameters.end()),
			return_type
		));

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

	/// Properties

	bool Class::is_abstract() const
	{
		return has(_impl->access, ClassAccess::Abstract);
	}

	Class::Version Class::get_version() const
	{
		return _impl->version;
	}

	void Class::set_version(Version v)
	{
		_impl->version = v;
	}

	Class::Superclass Class::get_superclass() const
	{
		return _impl->superclass;
	}

	void Class::set_superclass(Class c)
	{
		_impl->superclass = { c._impl.get() };
	}

	void Class::set_superclass(ExternalClass c)
	{
		_impl->superclass = { c };
	}

	/// Finalizing

	Result<ClassFile> Class::lower() const
	{
		ClassFile cf;
		ConstantPool pool;

		cf.major_version = _impl->version.major;
		cf.minor_version = _impl->version.minor;

		Result<u2> utf_res = Error::None;
#define TRY_INTERN_UTF8(x) pool.intern_utf8(x); if (!utf_res) { return utf_res.error(); }

		utf_res = TRY_INTERN_UTF8(_impl->name);
		cf.this_class = *utf_res;

		utf_res = TRY_INTERN_UTF8(_impl->superclass.name());
		cf.super_class = *utf_res;

		for (auto& field : _impl->fields) {
			utf_res = TRY_INTERN_UTF8(field.name);
			u2 name_index = *utf_res;

			utf_res = TRY_INTERN_UTF8(field.type.descriptor());
			u2 descriptor_index = *utf_res;

			cf.fields.emplace_back(
				field.access,
				name_index,
				descriptor_index
				// TODO: also lower attributes
			);
		}

		// TODO: abstract instruction lowering pre-pass?

		// TODO: lower methods here

		cf.constant_pool = pool;

		return Result<ClassFile>(cf);
	}

	Class::Class() = default;
	Class::~Class() = default;
	Class::Class(Class&&) = default;
	Class& Class::operator=(Class&&) = default;
}