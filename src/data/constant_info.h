#pragma once
#include "../detail/common.h"

namespace classcafe
{
    namespace Constant
    {
#define default_eq(T) bool operator==(T const&) const = default

        struct Unusable {
            static constexpr u1 tag = 0;
            default_eq(Unusable);
        };

        struct Utf8 {
            static constexpr u1 tag = 1;
            std::string bytes;
            default_eq(Utf8);
        };

        struct Integer { static constexpr u1 tag = 3; int32_t value; default_eq(Integer); };
        struct Float   { static constexpr u1 tag = 4; float value; default_eq(Float); };
        struct Long    { static constexpr u1 tag = 5; int64_t value; default_eq(Long); };   // takes two pool slots
        struct Double  { static constexpr u1 tag = 6; double value; default_eq(Double); };    // takes two pool slots

        struct String { static constexpr u1 tag = 8;  u2 string_index; default_eq(String); };
        struct MethodType { static constexpr u1 tag = 16; u2 descriptor_index; default_eq(MethodType); };

        struct NameAndType {
            static constexpr u1 tag = 12;
            u2 name_index;
            u2 descriptor_index;
            default_eq(NameAndType);
        };

        enum class RefKind : u1 {
            GetField = 1, GetStatic, PutField, PutStatic,
            InvokeVirtual, InvokeStatic, InvokeSpecial,
            NewInvokeSpecial, InvokeInterface
        };

        struct MethodHandle {
            static constexpr u1 tag = 15;
            RefKind reference_kind;
            u2 reference_index;
            default_eq(MethodHandle);
        };

        // Identical layouts share one template, but each alias is a
        // distinct type, so std::variant can still tell them apart.
        template <u1 T> struct NameRef {
            static constexpr u1 tag = T;
            u2 name_index;
            default_eq(NameRef);
        };
        using Class   = NameRef<7>;
        using Module  = NameRef<19>;   // module-info only
        using Package = NameRef<20>;   // module-info only

        template <u1 T> struct MemberRef {
            static constexpr u1 tag = T;
            u2 class_index;
            u2 name_and_type_index;
            default_eq(MemberRef);
        };
        using Fieldref           = MemberRef<9>;
        using Methodref          = MemberRef<10>;
        using InterfaceMethodref = MemberRef<11>;

        template <u1 T> struct BootstrapRef {
            static constexpr u1 tag = T;
            u2 bootstrap_method_attr_index;
            u2 name_and_type_index;
            default_eq(BootstrapRef);
        };
        using Dynamic       = BootstrapRef<17>;
        using InvokeDynamic = BootstrapRef<18>;

        using Info = std::variant<
            Unusable,
            Utf8, Integer, Float, Long, Double,
            Class, String,
            Fieldref, Methodref, InterfaceMethodref,
            NameAndType, MethodHandle, MethodType,
            Dynamic, InvokeDynamic, Module, Package>;

        inline bool takes_two_slots(Info const& c) {
            return std::holds_alternative<Long>(c)
                || std::holds_alternative<Double>(c);
        }
    } // Constant
} // classcafe