#pragma once
#include "../detail/common.h"
#include "access_flags.h"
#include "attribute_info.h"

namespace classcafe
{
    struct MethodInfo {
        MethodAccess   access_flags;
        u2             name_index;
        u2             descriptor_index;
        Vec<Attribute::Info> attributes;
    };
}