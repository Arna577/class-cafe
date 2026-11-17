#pragma once

#if CLASSCAFE_STD_PROVIDED // define if the includer defines 'import std;'

#elif CLASSCAFE_IMPORT_STD
import std;
#else
#   include <cstdint>
#   include <format>
#   include <iostream>
#   include <memory>
#   include <span>
#   include <string>
#   include <string_view>
#   include <variant>
#   include <vector>
#endif

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

// Stringifying

#define STRINGIZE(x) #x
#define TO_STRING(x) STRINGIZE(x)

#define LINE_STRING TO_STRING(__LINE__)

// Debug Break

#if defined(_MSC_VER)
#   define classcafe_debug_break __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#   define classcafe_debug_break __builtin_trap()
#elif defined(__has_builtin)
#   if __has_builtin(__builtin_trap)
#       define classcafe_debug_break __builtin_trap()
#   endif
#else
#   define classcafe_debug_break std::raise(SIGTRAP)
#endif

// Assertions

#ifdef DEBUG
#   define classcafe_unreachable                                                \
        do {                                                                    \
            ::std::cerr << "Unreachable hit at " __FILE__ ":" LINE_STRING "\n"; \
            classcafe_debug_break;                                              \
            ::std::abort();                                                     \
        } while (0)
#else
#   if defined(_MSC_VER)
#      define classcafe_unreachable __assume(false)
#   else
#      define classcafe_unreachable __builtin_unreachable()
#   endif
#endif

#ifdef DEBUG
#   define classcafe_assert(condition)                                \
        do {                                                          \
            if (!(condition)) {                                       \
                ::std::cerr << "Assertion Failed: (" #condition ")\n" \
                          "File: " __FILE__ "\n"                      \
                          "Line: " LINE_STRING "\n";                  \
                classcafe_debug_break;                                \
                ::std::abort();                                       \
            }                                                         \
        } while (0)
#else
#   define classcafe_assert(condition) ((condition) ? (void)0 : unreachable)
#endif