#pragma once
#include <cstdint>
#include <vector>

namespace classcafe {
    using u4 = std::uint32_t;
    using u2 = std::uint16_t;
    using u1 = std::uint8_t;

    template<typename T>
    using Vec = std::vector<T>;
}