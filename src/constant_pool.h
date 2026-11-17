#pragma once
#include "detail/common.h"
#include "detail/mutf8.h"
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

		void add_unusable() {
			add(Constant::Unusable());
		}

		u2 intern(Constant::Info&& info) {
			for (u2 i = 0; i < num_constants(); ++i)
				if (_constants[i] == info)
					return i;

			bool two_slots = Constant::takes_two_slots(info);

			add(std::move(info));

			if (two_slots)
				add_unusable();

			return last();
		}

		Result<u2> intern_utf8(std::string_view s) {
			std::string out;

			if (!detail::to_mutf8(s, out))
				return Result<u2>(Error::InvalidUTF8);

			if (out.size() > 0xffff)
				return Result<u2>(Error::StringTooLong);

			return Result<u2>(intern(Constant::Utf8(out)));
		}

		u2 intern_integer(int32_t i) {
			return intern(Constant::Integer(i));
		}

		u2 intern_float(float f) {
			return intern(Constant::Float(f));
		}

		u2 intern_long(int64_t l) {
			u2 out = intern(Constant::Long(l));
			return out;
		}

		u2 intern_double(double d) {
			u2 out = intern(Constant::Double(d));
			return out;
		}

		ConstantPool() { add_unusable(); }
	};
}