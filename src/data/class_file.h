#pragma once
#include "../detail/common.h"
#include "access_flags.h"
#include "attribute_info.h"
#include "constant_info.h"
#include "field_info.h"
#include "method_info.h"

namespace classcafe
{
    // https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
    struct ClassFile
    {
        static constexpr u4 magic = 0xCAFEBABE;
        u2             minor_version;
        u2             major_version;
        Vec<Constant::Info> constant_pool;
        ClassAccess    access_flags;
        u2             this_class;
        u2             super_class;
        Vec<u2> interfaces;
        Vec<FieldInfo> fields;
        Vec<MethodInfo> methods;
        Vec<Attribute::Info> attributes;
    };
}