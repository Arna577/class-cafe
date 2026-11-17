#pragma once
#include "detail/common.h"

namespace classcafe {
    enum class ClassAccess : u2 {
        Default = 0, // package-private class

        Public     = 0x0001,
        Final      = 0x0010,
        Super      = 0x0020,   // always set this on new classes
        Interface  = 0x0200,   // must also set Abstract
        Abstract   = 0x0400,
        Synthetic  = 0x1000,
        Annotation = 0x2000,   // must also set Interface
        Enum       = 0x4000,
        Module     = 0x8000,   // module-info only
    };

    enum class FieldAccess : u2 {
        Default = 0, // package-private mutable

        Public    = 0x0001,
        Private   = 0x0002,
        Protected = 0x0004,
        Static    = 0x0008,
        Final     = 0x0010,
        Volatile  = 0x0040,
        Transient = 0x0080,
        Synthetic = 0x1000,
        Enum      = 0x4000,
    };

    enum class MethodAccess : u2 {
        Default = 0, // package-private

        Public       = 0x0001,
        Private      = 0x0002,
        Protected    = 0x0004,
        Static       = 0x0008,
        Final        = 0x0010,
        Synchronized = 0x0020,
        Bridge       = 0x0040,
        Varargs      = 0x0080,
        Native       = 0x0100,
        Abstract     = 0x0400,
        Strict       = 0x0800,   // only meaningful for class versions 46-60
        Synthetic    = 0x1000,
    };

    // InnerClasses attribute entries
    enum class InnerClassAccess : u2 {
        Public     = 0x0001,
        Private    = 0x0002,
        Protected  = 0x0004,
        Static     = 0x0008,
        Final      = 0x0010,
        Interface  = 0x0200,
        Abstract   = 0x0400,
        Synthetic  = 0x1000,
        Annotation = 0x2000,
        Enum       = 0x4000,
    };

    // MethodParameters attribute entries
    enum class ParameterAccess : u2 {
        Default = 0, // mutable

        Final     = 0x0010,
        Synthetic = 0x1000,
        Mandated  = 0x8000,
    };

    // Define flag operations
    #define FLAG_OP(op)                                                  \
        template <class E> requires enable_flag_ops<E>                   \
        constexpr E operator op(E a, E b) {                              \
            using U = std::underlying_type_t<E>;                         \
            return static_cast<E>(static_cast<U>(a) op static_cast<U>(b)); \
        }

    // Enable bitwise operations
    template <class E, class... Ts>
    inline constexpr bool is_one_of = (std::is_same_v<E, Ts> || ...);

    template <class E> inline constexpr bool enable_flag_ops =
        is_one_of<E, ClassAccess, FieldAccess, MethodAccess,
        InnerClassAccess, ParameterAccess>;

    FLAG_OP(|)
    FLAG_OP(&)
    #undef FLAG_OP

    template <class E> requires enable_flag_ops<E>
    constexpr bool has(E value, E flag) { return (value & flag) == flag; }
}