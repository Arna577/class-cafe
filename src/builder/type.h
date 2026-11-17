#pragma once
#include "../detail/common.h"

namespace classcafe
{
    enum class BaseType : u1 {
        Byte, Char, Double, Float, Int, Long, Short, Boolean,
        Void,
        Reference,   // class_name is valid
    };

    struct JVMType {
        BaseType base;
        u1 dims;                  // Array dimensions
        std::string class_name;   // only meaningful if base == Reference, e.g. "java/lang/String"

        constexpr JVMType(BaseType b, u1 d = 0) : base(b), dims(d), class_name() {}
        constexpr JVMType(std::string_view cls, u1 d = 0) : base(BaseType::Reference), dims(d), class_name(cls) {}

        constexpr bool is_array() const { return dims > 0; }
        constexpr bool is_reference() const { return dims > 0 || base == BaseType::Reference; }

        constexpr JVMType element_type() const {
            JVMType copy = *this;
            --copy.dims;
            return copy;
        }

        constexpr JVMType& array() {
            ++dims;
            return *this;
        }

        friend constexpr bool operator==(JVMType const& a, JVMType const& b) {
            return a.base == b.base && a.dims == b.dims &&
                (a.base != BaseType::Reference || a.class_name == b.class_name);
        }
    };
}