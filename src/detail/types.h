#pragma once
#include <cstdint>
#include <vector>

namespace classcafe {
    using u4 = std::uint32_t;
    using u2 = std::uint16_t;
    using u1 = std::uint8_t;

    using i4 = std::int32_t;
    using i2 = std::int16_t;
    using i1 = std::int8_t;

    template<typename T>
    using Vec = std::vector<T>;
}