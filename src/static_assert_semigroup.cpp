// ~/~ begin <<docs/modern-cpp.md#src/static_assert_semigroup.cpp>>[init]
#include <concepts>

// ~/~ begin <<docs/modern-cpp.md#semigroup-unit-decl>>[init]
template <typename T>
constexpr T unit();
// ~/~ end
// ~/~ begin <<docs/modern-cpp.md#semigroup-concept>>[init]
template <typename T>
concept SemiGroup = requires (T const &a, T const &b) {
    { a * b } -> std::convertible_to<T>;
    { unit<T>() } -> std::convertible_to<T>;
};
// ~/~ end
// ~/~ begin <<docs/modern-cpp.md#semigroup-concept>>[1]
template <typename S> requires SemiGroup<S>
S power(S const a, int n) {
    S result = unit<S>();
    for (int i = 0; i < n; ++i) {
        result = result * a;
    }
    return result;
}
// ~/~ end

static_assert(SemiGroup<int>);
static_assert(SemiGroup<char const *>);
// ~/~ end
