---
icon: material/language-cpp
---

# Modern C++

C++ has undergone some major advancements since last I looked at it.

## Hello World

The Hello World program has changed! We now have `std::print`.

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

Multiplication operator is always there for us to overload. The `unit` factory needs to be declared somewhere:

```c++
//| id: semigroup-unit-decl
template <typename T>
T unit();
```

The concept is declared as follows.

```c++
//| id: semigroup-concept
template <typename T>
concept SemiGroup = requires (T const &a, T const &b) {
    { a * b } -> std::convertible_to<T>;
    { unit<T>() } -> std::convertible_to<T>;
};
```

As C++ syntax goes, this is actually quite nice. (Wait until you read the error messages...) With that concept defined, we can implement the `power` function:

```c++
//| id: semigroup-concept
template <typename S> requires SemiGroup<S>
S power(S const a, int n) {
    S result = unit<S>();
    for (int i = 0; i < n; ++i) {
        result = result * a;
    }
    return result;
}
```

### Example: Strings
For example, we can implement a semi-group on strings:

```c++
//| id: string-semigroup
template <>
std::string unit<std::string>() {
    return std::string{};
}

std::string operator*(std::string const &a, std::string const &b) {
    return a + b;
}
```

Somehow, my code wouldn't compile unless I defined these overloaded implementations before defining the `SemiGroup` concept.

### Example: Quarter Rotations 
The (full) group of quarter rotations. We can use modular arithmatic to implement the `*` operator.

```c++
//| id: rotation-semigroup
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
```

Note that to convert a `Rotation` to an integer, an implicit cast suffices. Not so for the other way around. Also, now that we defined our own type, the compiler is fine with having the `Rotation` instance for the `SemiGroup` concept defined after the concept itself. Must be some weird voodoo.

## Defining a formatter
With the new `std::print` functionality, we may want to implement a `std::formatter` for our `Rotation` type. This implementation is adapted from [CPP Reference](https://en.cppreference.com/w/cpp/utility/format/formatter.html). I'll make it so that you can print a rotation in human readable (all lower case, space separated), snake upper case (all upper case with underscores), or as a plain integer. So:

```c++
std::print("{}", LEFT_TURN);     //=> LEFT_TURN
std::print("{:h}", LEFT_TURN);   //=> left turn
std::print("{:s}", RIGHT_TURN);  //=> RIGHT_TURN
std::print("{:i}", NO_TURN);     //=> 0
```

To make this work, we need to define `std::formatter<Rotation>`.

```c++
//| id: rotation-formatter
<<rotation-formatter-helpers>>

template<>
struct std::formatter<Rotation> {
    <<rotation-formatter-data>>

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext &ctx) {
        <<rotation-formatter-parser>>
    }

    template <class FmtContext>
    FmtContext::iterator format(Rotation r, FmtContext &ctx) const {
        <<rotation-formatter-impl>>
    }
};
```

### Formatter data
We have three different styles of formatting, so we need to store that information in an enum:

```c++
//| id: rotation-formatter-data
enum Style {
    HUMAN_READABLE, SNAKE_CASE, INTEGER
} style = SNAKE_CASE;
```

### Formatter parser
Note that this method is `constexpr`. That means that the resulting `std::formatter` object is known at compile time.
We are given an iterator over the characters in the format string. It is our duty to pass back the iterator on the position of the closing curly brace after we're done.

```c++
//| id: rotation-formatter-parser
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
```

### Formatter

```c++
//| id: rotation-formatter-impl
switch (style) {
    case HUMAN_READABLE:
        return std::formatter<char const *>{}.format(human_readable_fmt(r), ctx);
    case SNAKE_CASE:
        return std::formatter<char const *>{}.format(snake_case_fmt(r), ctx);
    case INTEGER:
        return std::formatter<int>{}.format(static_cast<int>(r), ctx);
}
throw std::format_error("Illegal state in Rotation formatter.");
```

### Helper functions
Here, we've put the actual string conversion in some small helper functions.

```c++
//| id: rotation-formatter-helpers
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
```

### Concepts and String formatting example

```c++
//| file: src/concepts.cpp
#include <cstdlib>
#include <print>
#include <string>
#include <concepts>

<<semigroup-unit-decl>>
<<string-semigroup>>
<<semigroup-concept>>
<<rotation-semigroup>>
<<rotation-formatter>>

int main() {
    std::println("{:h}", NO_TURN * LEFT_TURN);
    std::println("{}", power(std::string("Jetzt! "), 3));
    std::println("{}", power(LEFT_TURN, 3));
    std::println("turn around twice: {0:h}, that's a {0:i} for me.", power(HALF_TURN, 2));
    return EXIT_SUCCESS;
}
```
