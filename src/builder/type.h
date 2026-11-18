#pragma once
#include "../detail/common.h"

namespace classcafe
{
    enum class BaseType : u1 {
        // newarray type ids
        Boolean = 4, Char, Float, Double, Byte, Short, Int, Long,
        Void,
        Reference,   // class_name is valid
    };

    char base_type_descriptor(BaseType base) {
        switch (base) {
        case BaseType::Boolean: return 'Z';
        case BaseType::Char: return 'C';
        case BaseType::Float: return 'F';
        case BaseType::Double: return 'D';
        case BaseType::Byte: return 'B';
        case BaseType::Short: return 'S';
        case BaseType::Int: return 'I';
        case BaseType::Long: return 'J';
        case BaseType::Void: return 'V';
        }
        classcafe_unreachable;
    }

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
        
        std::string descriptor() const {
            std::string out;
            bool is_ref = base == BaseType::Reference;

            out.reserve(dims + is_ref ? class_name.size() + 1 : 1);

            for (size_t i = 0; i < dims; ++i) out.push_back('[');

            if (is_ref) {
                out.push_back('L');
                out.append(class_name);
            }
            else {
                out.push_back(base_type_descriptor(base));
            }

            return out;
        }
    };
}