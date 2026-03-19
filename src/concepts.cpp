// ~/~ begin <<docs/modern-cpp.md#src/concepts.cpp>>[init]
#include <cstdlib>
#include <print>
#include <string>
#include <concepts>

// ~/~ begin <<docs/modern-cpp.md#semigroup-unit-decl>>[init]
template <typename T>
constexpr T unit();
// ~/~ end
// ~/~ begin <<docs/modern-cpp.md#string-semigroup>>[init]
template <>
std::string unit<std::string>() {
    return std::string{};
}

std::string operator*(std::string const &a, std::string const &b) {
    return a + b;
}
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
// ~/~ begin <<docs/modern-cpp.md#rotation-semigroup>>[init]
enum Rotation {
    NO_TURN, RIGHT_TURN, HALF_TURN, LEFT_TURN
};

Rotation operator*(Rotation a, Rotation b) {
    return static_cast<Rotation>((a + b) % 4);
}

template <>
constexpr Rotation unit<Rotation>() {
    return NO_TURN;
}
// ~/~ end
// ~/~ begin <<docs/modern-cpp.md#rotation-formatter>>[init]
// ~/~ begin <<docs/modern-cpp.md#rotation-formatter-helpers>>[init]
constexpr char const *human_readable_fmt(Rotation r) {
    switch (r) {
        case NO_TURN: return "no turn";
        case RIGHT_TURN: return "right turn";
        case HALF_TURN: return "half turn";
        case LEFT_TURN: return "left turn";
    }
    return "<illegal rotation datum>";
}

constexpr char const *snake_case_fmt(Rotation r) {
    switch (r) {
        case NO_TURN: return "NO_TURN";
        case RIGHT_TURN: return "RIGHT_TURN";
        case HALF_TURN: return "HALF_TURN";
        case LEFT_TURN: return "LEFT_TURN";
    }
    return "<illegal rotation datum>";
}
// ~/~ end

template<>
struct std::formatter<Rotation> {
    // ~/~ begin <<docs/modern-cpp.md#rotation-formatter-data>>[init]
    enum Style {
        HUMAN_READABLE, SNAKE_CASE, INTEGER
    } style = SNAKE_CASE;
    // ~/~ end

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext &ctx) {
        // ~/~ begin <<docs/modern-cpp.md#rotation-formatter-parser>>[init]
        auto it = ctx.begin();
        if (it == ctx.end()) {
            return it;
        }
        
        switch (*it) {
            case 'h':
                style = HUMAN_READABLE; ++it; break;
            case 's':
                style = SNAKE_CASE; ++it; break;
            case 'i':
                style = INTEGER; ++it; break;
        }
        
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format args for Rotation.");
        }
        
        return it;
        // ~/~ end
    }

    template <class FmtContext>
    FmtContext::iterator format(Rotation r, FmtContext &ctx) const {
        // ~/~ begin <<docs/modern-cpp.md#rotation-formatter-impl>>[init]
        switch (style) {
            case HUMAN_READABLE:
                return std::formatter<char const *>{}.format(human_readable_fmt(r), ctx);
            case SNAKE_CASE:
                return std::formatter<char const *>{}.format(snake_case_fmt(r), ctx);
            case INTEGER:
                return std::formatter<int>{}.format(static_cast<int>(r), ctx);
        }
        throw std::format_error("Illegal state in Rotation formatter.");
        // ~/~ end
    }
};
// ~/~ end

int main() {
    std::println("{:h}", NO_TURN * LEFT_TURN);
    std::println("{}", power(std::string("Jetzt! "), 3));
    std::println("{}", power(LEFT_TURN, 3));
    std::println("turn around twice: {0:h}, that's a {0:i} for me.", power(HALF_TURN, 2));
    return EXIT_SUCCESS;
}
// ~/~ end
