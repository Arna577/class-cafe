#pragma once
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <span>

namespace classcafe {
    using u4 = std::uint32_t;
    using u2 = std::uint16_t;
    using u1 = std::uint8_t;

    using i8 = std::int64_t;
    using i4 = std::int32_t;
    using i2 = std::int16_t;
    using i1 = std::int8_t;

    template<typename T>
    using Vec = std::vector<T>;

    template <class T> using Slice = std::span<const T>; // read-only view, like ArrayRef<T>
    template <class T> using SliceMut = std::span<T>;    // mutable view
}