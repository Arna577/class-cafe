#pragma once
#include "access_flags.h"
#include "attribute_info.h"
#include "detail/common.h"

namespace classcafe
{
    struct FieldInfo {
        FieldAccess    access_flags;
        u2             name_index;
        u2             descriptor_index;
        Vec<Attribute::Info> attributes;
    };
}