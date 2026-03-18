---
icon: material/language-cpp
---

# Modern C++

C++ has undergone some major advancements since last I looked at it.

## Hello World

The Hello World program has changed!

```meson
#| id: meson-executables
executable('hello', 'src/hello_world.cpp')
```

```c++
//| file: src/hello_world.cpp
#include <cstdlib>
#include <print>

int main() {
    std::println("Hello, World!");
    return EXIT_SUCCESS;
}
```

## Concepts

```meson
#| id: meson-executables
executable('jetzt', 'src/concepts.cpp')
```

In this demo, I'll create a concept of a semi-group. A semi-group is a class that defines a binomial `*` such that the result has the same type, and a `unit` element that provides the identity `k * unit == k` for all `k` in the semi-group.

```c++
//| file: src/concepts.cpp
#include <cstdlib>
#include <iostream>
#include <print>
#include <string>
#include <concepts>

template <typename T>
T unit();

template <>
std::string unit<std::string>() {
    return std::string{};
}

std::string operator*(std::string const &a, std::string const &b) {
    return a + b;
}

template <typename T>
concept SemiGroup = requires (T const &a, T const &b) {
    { a * b } -> std::convertible_to<T>;
    { unit<T>() } -> std::convertible_to<T>;
};

template <typename S> requires SemiGroup<S>
S power(S const a, int n) {
    S result = unit<S>();
    for (int i = 0; i < n; ++i) {
        result = result * a;
    }
    return result;
}

enum Rotation {
    STRAIGHT, RIGHT, HALF_TURN, LEFT
};

constexpr char const *human_readable_fmt(Rotation r) {
    switch (r) {
        case STRAIGHT: return "straight";
        case RIGHT: return "right";
        case HALF_TURN: return "half turn";
        case LEFT: return "left";
    }
    return "<illegal rotation datum>";
}

constexpr char const *snake_case_fmt(Rotation r) {
    switch (r) {
        case STRAIGHT: return "STRAIGHT";
        case RIGHT: return "RIGHT";
        case HALF_TURN: return "HALF_TURN";
        case LEFT: return "LEFT";
    }
    return "<illegal rotation datum>";
}

// Adapted from https://en.cppreference.com/w/cpp/utility/format/formatter.html
template<>
struct std::formatter<Rotation> {
    enum Style {
        HUMAN_READABLE, SNAKE_CASE, INTEGER
    } style = SNAKE_CASE;

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
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
    }

    template <class FmtContext>
    FmtContext::iterator format(Rotation r, FmtContext &ctx) const {
        switch (style) {
            case HUMAN_READABLE:
                return std::formatter<char const *>{}.format(human_readable_fmt(r), ctx);
            case SNAKE_CASE:
                return std::formatter<char const *>{}.format(snake_case_fmt(r), ctx);
            case INTEGER:
                return std::formatter<int>{}.format(static_cast<int>(r), ctx);
        }
        throw std::format_error("Illegal state in Rotation formatter.");
    }
};

Rotation operator*(Rotation a, Rotation b) {
    return static_cast<Rotation>((a + b) % 4);
}

template <>
constexpr Rotation unit<Rotation>() {
    return STRAIGHT;
}

int main() {
    std::println("{:h}", STRAIGHT * LEFT);
    std::println("{}", power(std::string("Jetzt! "), 3));
    std::println("{}", power(LEFT, 3));
    std::println("turn around twice: {0:h}, that's a {0:i} for me.", power(HALF_TURN, 2));
    return EXIT_SUCCESS;
}
```
