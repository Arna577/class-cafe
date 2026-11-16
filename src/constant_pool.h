#pragma once
#include "detail/mutf8.h"
#include "detail/types.h"
#include "constant_info.h"
#include "result.h"

namespace classcafe
{
	class ConstantPool {
		Vec<Constant::Info> _constants;
	public:
		/*
		Utf8, Integer, Float, Long, Double,
        Class, String,
        Fieldref, Methodref, InterfaceMethodref,
        NameAndType, MethodHandle, MethodType,
        Dynamic, InvokeDynamic, Module, Package
		*/

		size_t num_constants() const { return _constants.size(); }
		size_t last() const { return _constants.size() - 1; }
		bool overflowed() const { return _constants.size() > 0xffff; }

		void add(Constant::Info&& info) {
			_constants.push_back(info);
		}

		u2 intern(Constant::Info&& info) {
			for (u2 i = 0; i < num_constants(); ++i) {
				if (_constants[i] == info) return i;
			}
			add(std::move(info));
			return last();
		}

		void add_unusable() {
			add(Constant::Unusable());
		}

		Result<u2> intern_utf8(std::string_view s) {
			std::string out;

			if (!detail::to_mutf8(s, out))
				return Result<u2>(Error::InvalidUTF8);

			if (out.size() > 0xffff)
				return Result<u2>(Error::StringTooLong);

			intern(Constant::Utf8(out));

			return Result<u2>(last());
		}

		u2 intern_integer(int32_t i) {
			return intern(Constant::Integer(i));
		}

		u2 intern_float(float f) {
			return intern(Constant::Float(f));
		}

		u2 intern_long(int64_t l) {
			u2 out = intern(Constant::Long(l));
			add_unusable();
			return out;
		}

		u2 intern_double(double d) {
			u2 out = intern(Constant::Double(d));
			add_unusable();
			return out;
		}
	};
}