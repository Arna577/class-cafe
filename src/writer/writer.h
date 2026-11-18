#pragma once
#include "../detail/common.h"
#include "../data/class_file.h"
#include "../errors.h"

namespace classcafe
{
	namespace detail
	{
		template <class... Fs> struct Overloaded : Fs... { using Fs::operator()...; };
		template <class... Fs> Overloaded(Fs...) -> Overloaded<Fs...>;
	}

// Error forwarding for Error-returning functions
#define CLASSCAFE_TRY(expr)                                                   \
	do {                                                                      \
		::classcafe::Error classcafe_e_ = (expr);                             \
		if (classcafe_e_ != ::classcafe::Error::None) return classcafe_e_;    \
	} while (0)

	// Serializes a ClassFile to its binary form. Everything is big-endian.
	//
	// Usage:
	//     ByteWriter w;
	//     if (Error e = w.encode(cf); e != Error::None) { ... }
	//     w.dump(file);
	//
	// encode() replaces the contents of `bytes`, and leaves it empty on failure, so a
	// half-written class can never be dumped.
	//
	// The writer does not check that indices point at the right kind of constant;
	// it only checks what it needs to produce a well-formed byte stream;
	// counts that fit, a well-formed pool layout, valid Code size, valid frame shapes.
	struct ByteWriter {
		Vec<u1> bytes;

		/// Primitives

		void U1(u1 val) { bytes.push_back(val); }
		void U2(u2 val) {
			U1(u1(val >> 8));
			U1(u1(val));
		}
		void U4(u4 val) {
			U2(u2(val >> 16));
			U2(u2(val));
		}
		void U8(std::uint64_t val) {
			U4(u4(val >> 32));
			U4(u4(val));
		}

		void Raw(Slice<u1> data) {
			bytes.insert(bytes.end(), data.begin(), data.end());
		}
		void Chars(std::string_view s) {
			for (char c : s) U1(static_cast<u1>(c));
		}

		void patch_u4(size_t at, u4 val) {
			bytes[at + 0] = u1(val >> 24);
			bytes[at + 1] = u1(val >> 16);
			bytes[at + 2] = u1(val >> 8);
			bytes[at + 3] = u1(val);
		}

		/// Counts and lengths

		Error count_u1(size_t n) {
			if (n > 0xFF) return Error::TooManyItems;
			U1(u1(n));
			return Error::None;
		}

		Error count_u2(size_t n) {
			if (n > 0xFFFF) return Error::TooManyItems;
			U2(u2(n));
			return Error::None;
		}

		// Writes a u4 length, runs body(), then goes back and fills the length in.
		template <class F>
		Error with_u4_length(F&& body) {
			size_t at = bytes.size();
			U4(0);
			CLASSCAFE_TRY(body());
			size_t length = bytes.size() - at - 4;
			if (length > 0xFFFFFFFFull) return Error::TooLong;
			patch_u4(at, u4(length));
			return Error::None;
		}

		/// Constant pool

		Error constant(Constant::Info const& info) {
			return std::visit(detail::Overloaded{
				[&](Constant::Unusable const&) -> Error { return Error::None; },

				[&](Constant::Utf8 const& v) -> Error {
					if (v.bytes.size() > 0xFFFF) return Error::StringTooLong;
					// Modified UTF-8 never contains NUL, or bytes in 0xF0..0xFF.
					for (char c : v.bytes) {
						u1 b = static_cast<u1>(c);
						if (b == 0 || b >= 0xF0) return Error::InvalidUTF8;
					}
					U1(v.tag);
					U2(u2(v.bytes.size()));
					Chars(v.bytes);
					return Error::None;
				},

				[&](Constant::Integer const& v) -> Error {
					U1(v.tag);
					U4(std::bit_cast<u4>(v.value));
					return Error::None;
				},
				[&](Constant::Float const& v) -> Error {
					U1(v.tag);
					U4(std::bit_cast<u4>(v.value));
					return Error::None;
				},
				[&](Constant::Long const& v) -> Error {
					U1(v.tag);
					U8(std::bit_cast<std::uint64_t>(v.value));
					return Error::None;
				},
				[&](Constant::Double const& v) -> Error {
					U1(v.tag);
					U8(std::bit_cast<std::uint64_t>(v.value));
					return Error::None;
				},

				[&](Constant::String const& v) -> Error {
					U1(v.tag); U2(v.string_index);
					return Error::None;
				},
				[&](Constant::MethodType const& v) -> Error {
					U1(v.tag); U2(v.descriptor_index);
					return Error::None;
				},
				[&](Constant::NameAndType const& v) -> Error {
					U1(v.tag); U2(v.name_index); U2(v.descriptor_index);
					return Error::None;
				},
				[&](Constant::MethodHandle const& v) -> Error {
					U1(v.tag); U1(u1(v.reference_kind)); U2(v.reference_index);
					return Error::None;
				},

				// Class, Module, Package
				[&]<u1 T>(Constant::NameRef<T> const& v) -> Error {
					U1(v.tag); U2(v.name_index);
					return Error::None;
				},
				// Fieldref, Methodref, InterfaceMethodref
				[&]<u1 T>(Constant::MemberRef<T> const& v) -> Error {
					U1(v.tag); U2(v.class_index); U2(v.name_and_type_index);
					return Error::None;
				},
				// Dynamic, InvokeDynamic
				[&]<u1 T>(Constant::BootstrapRef<T> const& v) -> Error {
					U1(v.tag); U2(v.bootstrap_method_attr_index); U2(v.name_and_type_index);
					return Error::None;
				},
			}, info);
		}

		// `pool` is indexed the way the class file is: pool[0] is Unusable, and a
		// Long/Double is followed by an Unusable. constant_pool_count == pool.size().
		Error constant_pool(Vec<Constant::Info> const& pool) {
			if (pool.empty() || !std::holds_alternative<Constant::Unusable>(pool[0]))
				return Error::BadConstantPool;
			if (pool.size() > 0xFFFF)
				return Error::TooManyItems;

			U2(u2(pool.size()));

			for (size_t i = 1; i < pool.size(); ++i) {
				Constant::Info const& c = pool[i];

				// An Unusable here is not the second half of a Long/Double.
				if (std::holds_alternative<Constant::Unusable>(c))
					return Error::BadConstantPool;

				CLASSCAFE_TRY(constant(c));

				if (Constant::takes_two_slots(c)) {
					if (i + 1 >= pool.size() || !std::holds_alternative<Constant::Unusable>(pool[i + 1]))
						return Error::BadConstantPool;
					++i;
				}
			}
			return Error::None;
		}

		/// Stack map

		Error vtype(StackMap::VType const& t) {
			return std::visit(detail::Overloaded{
				[&](StackMap::Object const& o) -> Error {
					U1(o.tag); U2(o.class_index);
					return Error::None;
				},
				[&](StackMap::Uninitialized const& u) -> Error {
					U1(u.tag); U2(u.new_offset);
					return Error::None;
				},
				[&](auto const& simple) -> Error {
					U1(simple.tag);
					return Error::None;
				},
			}, t);
		}

		Error vtypes_counted(Vec<StackMap::VType> const& types) {
			CLASSCAFE_TRY(count_u2(types.size()));
			for (auto const& t : types) CLASSCAFE_TRY(vtype(t));
			return Error::None;
		}

		Error frame(StackMap::Frame const& f) {
			return std::visit(detail::Overloaded{
				[&](StackMap::Same const& s) -> Error {
					if (s.offset_delta <= 63) {
						U1(u1(s.offset_delta));                // same_frame, 0..63
					} else {
						U1(251); U2(s.offset_delta);           // same_frame_extended
					}
					return Error::None;
				},
				[&](StackMap::SameLocals1 const& s) -> Error {
					if (s.offset_delta <= 63) {
						U1(u1(64 + s.offset_delta));           // same_locals_1_stack_item_frame, 64..127
					} else {
						U1(247); U2(s.offset_delta);           // ..._extended
					}
					return vtype(s.stack);
				},
				[&](StackMap::Chop const& c) -> Error {
					if (c.k < 1 || c.k > 3) return Error::BadStackMapFrame;
					U1(u1(251 - c.k));                         // chop_frame, 248..250
					U2(c.offset_delta);
					return Error::None;
				},
				[&](StackMap::Append const& a) -> Error {
					if (a.locals.empty() || a.locals.size() > 3) return Error::BadStackMapFrame;
					U1(u1(251 + a.locals.size()));             // append_frame, 252..254
					U2(a.offset_delta);
					for (auto const& t : a.locals) CLASSCAFE_TRY(vtype(t));
					return Error::None;
				},
				[&](StackMap::Full const& full) -> Error {
					U1(255);
					U2(full.offset_delta);
					CLASSCAFE_TRY(vtypes_counted(full.locals));
					return vtypes_counted(full.stack);
				},
			}, f);
		}

		/// Annotations

		Error annotation(Annotations::Annotation const& a) {
			U2(a.type_index);
			CLASSCAFE_TRY(count_u2(a.pairs.size()));
			for (auto const& p : a.pairs) {
				U2(p.name_index);
				CLASSCAFE_TRY(element_value(p.value));
			}
			return Error::None;
		}

		Error annotations_counted(Vec<Annotations::Annotation> const& list) {
			CLASSCAFE_TRY(count_u2(list.size()));
			for (auto const& a : list) CLASSCAFE_TRY(annotation(a));
			return Error::None;
		}

		Error element_value(Annotations::ElementValue const& ev) {
			return std::visit(detail::Overloaded{
				[&](Annotations::Const const& c) -> Error {
					U1(u1(c.kind)); U2(c.const_value_index);
					return Error::None;
				},
				[&](Annotations::EnumConst const& e) -> Error {
					U1('e'); U2(e.type_name_index); U2(e.const_name_index);
					return Error::None;
				},
				[&](Annotations::ClassInfo const& c) -> Error {
					U1('c'); U2(c.class_info_index);
					return Error::None;
				},
				[&](Annotations::Annotation const& a) -> Error {
					U1('@');
					return annotation(a);
				},
				[&](Annotations::ArrayValue const& arr) -> Error {
					U1('[');
					CLASSCAFE_TRY(count_u2(arr.values.size()));
					for (auto const& v : arr.values) CLASSCAFE_TRY(element_value(v));
					return Error::None;
				},
			}, ev.value);
		}

		Error parameter_annotations(Vec<Vec<Annotations::Annotation>> const& params) {
			CLASSCAFE_TRY(count_u1(params.size()));
			for (auto const& list : params) CLASSCAFE_TRY(annotations_counted(list));
			return Error::None;
		}

		/// Attributes

		Error attributes(Vec<Attribute::Info> const& list) {
			CLASSCAFE_TRY(count_u2(list.size()));
			for (auto const& a : list) CLASSCAFE_TRY(attribute(a));
			return Error::None;
		}

		Error attribute(Attribute::Info const& a) {
			U2(a.attribute_name_index);
			return with_u4_length([&] {
				return std::visit([&](auto const& body) -> Error { return attribute_body(body); }, a.kind);
			});
		}

		Error attribute_body(Attribute::ConstantValue const& a) {
			U2(a.constantvalue_index);
			return Error::None;
		}

		Error attribute_body(Attribute::Code const& a) {
			if (a.code.empty() || a.code.size() > 0xFFFF) return Error::BadCode;

			U2(a.max_stack);
			U2(a.max_locals);
			U4(u4(a.code.size()));
			Raw(a.code);

			CLASSCAFE_TRY(count_u2(a.exception_table.size()));
			for (auto const& e : a.exception_table) {
				U2(e.start_pc);
				U2(e.end_pc);
				U2(e.handler_pc);
				U2(e.catch_type);
			}

			return attributes(a.attributes);
		}

		Error attribute_body(Attribute::StackMapTable const& a) {
			CLASSCAFE_TRY(count_u2(a.entries.size()));
			for (auto const& f : a.entries) CLASSCAFE_TRY(frame(f));
			return Error::None;
		}

		Error attribute_body(Attribute::Exceptions const& a) {
			CLASSCAFE_TRY(count_u2(a.exception_index_table.size()));
			for (u2 i : a.exception_index_table) U2(i);
			return Error::None;
		}

		Error attribute_body(Attribute::InnerClasses const& a) {
			CLASSCAFE_TRY(count_u2(a.classes.size()));
			for (auto const& c : a.classes) {
				U2(c.inner_class_info_index);
				U2(c.outer_class_info_index);
				U2(c.inner_name_index);
				U2(static_cast<u2>(c.inner_class_access_flags));
			}
			return Error::None;
		}

		Error attribute_body(Attribute::EnclosingMethod const& a) {
			U2(a.class_index);
			U2(a.method_index);
			return Error::None;
		}

		Error attribute_body(Attribute::Synthetic const&) { return Error::None; }
		Error attribute_body(Attribute::Deprecated const&) { return Error::None; }

		Error attribute_body(Attribute::Signature const& a) {
			U2(a.signature_index);
			return Error::None;
		}

		Error attribute_body(Attribute::SourceFile const& a) {
			U2(a.sourcefile_index);
			return Error::None;
		}

		// No count: the data runs to the end of the attribute.
		Error attribute_body(Attribute::SourceDebugExtension const& a) {
			Raw(a.debug_extension);
			return Error::None;
		}

		Error attribute_body(Attribute::LineNumberTable const& a) {
			CLASSCAFE_TRY(count_u2(a.line_number_table.size()));
			for (auto const& e : a.line_number_table) {
				U2(e.start_pc);
				U2(e.line_number);
			}
			return Error::None;
		}

		Error attribute_body(Attribute::LocalVariableTable const& a) {
			CLASSCAFE_TRY(count_u2(a.local_variable_table.size()));
			for (auto const& e : a.local_variable_table) {
				U2(e.start_pc);
				U2(e.length);
				U2(e.name_index);
				U2(e.descriptor_index);
				U2(e.index);
			}
			return Error::None;
		}

		Error attribute_body(Attribute::LocalVariableTypeTable const& a) {
			CLASSCAFE_TRY(count_u2(a.local_variable_type_table.size()));
			for (auto const& e : a.local_variable_type_table) {
				U2(e.start_pc);
				U2(e.length);
				U2(e.name_index);
				U2(e.signature_index);
				U2(e.index);
			}
			return Error::None;
		}

		Error attribute_body(Attribute::RuntimeVisibleAnnotations const& a) {
			return annotations_counted(a.annotations);
		}

		Error attribute_body(Attribute::RuntimeInvisibleAnnotations const& a) {
			return annotations_counted(a.annotations);
		}

		Error attribute_body(Attribute::RuntimeVisibleParameterAnnotations const& a) {
			return parameter_annotations(a.parameter_annotations);
		}

		Error attribute_body(Attribute::RuntimeInvisibleParameterAnnotations const& a) {
			return parameter_annotations(a.parameter_annotations);
		}

		Error attribute_body(Attribute::AnnotationDefault const& a) {
			return element_value(a.element_value);
		}

		Error attribute_body(Attribute::BootstrapMethods const& a) {
			CLASSCAFE_TRY(count_u2(a.bootstrap_methods.size()));
			for (auto const& m : a.bootstrap_methods) {
				U2(m.bootstrap_method_ref);
				CLASSCAFE_TRY(count_u2(m.bootstrap_arguments.size()));
				for (u2 arg : m.bootstrap_arguments) U2(arg);
			}
			return Error::None;
		}

		Error attribute_body(Attribute::MethodParameters const& a) {
			CLASSCAFE_TRY(count_u1(a.parameters.size()));
			for (auto const& p : a.parameters) {
				U2(p.name_index);
				U2(p.access_flags);
			}
			return Error::None;
		}

		/// Members

		Error field(FieldInfo const& f) {
			U2(static_cast<u2>(f.access_flags));
			U2(f.name_index);
			U2(f.descriptor_index);
			return attributes(f.attributes);
		}

		Error method(MethodInfo const& m) {
			U2(static_cast<u2>(m.access_flags));
			U2(m.name_index);
			U2(m.descriptor_index);
			return attributes(m.attributes);
		}

		/// Whole class

		Error encode(ClassFile const& cf) {
			bytes.clear();
			Error e = encode_class(cf);
			if (e != Error::None) bytes.clear();
			return e;
		}

		Error encode_class(ClassFile const& cf) {
			U4(ClassFile::magic);
			U2(cf.minor_version);
			U2(cf.major_version);

			CLASSCAFE_TRY(constant_pool(cf.constant_pool));

			U2(static_cast<u2>(cf.access_flags));
			U2(cf.this_class);
			U2(cf.super_class);

			CLASSCAFE_TRY(count_u2(cf.interfaces.size()));
			for (u2 i : cf.interfaces) U2(i);

			CLASSCAFE_TRY(count_u2(cf.fields.size()));
			for (auto const& f : cf.fields) CLASSCAFE_TRY(field(f));

			CLASSCAFE_TRY(count_u2(cf.methods.size()));
			for (auto const& m : cf.methods) CLASSCAFE_TRY(method(m));

			return attributes(cf.attributes);
		}

		void dump(std::ostream& os) const {
			os.write(reinterpret_cast<const char*>(bytes.data()), std::streamsize(bytes.size()));
		}
	};

#undef CLASSCAFE_TRY
}