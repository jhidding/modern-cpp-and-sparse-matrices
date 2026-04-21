// ~/~ begin <<docs/eigen.md#src/random_values.cpp>>[init]
#include <cstdlib>
#include <print>
#include <random>

int main() {
    std::println("Some die throws ...");
    std::mt19937_64 r;
    std::uniform_int_distribution<int> die(1, 6);

    for (unsigned i = 0; i < 10; ++i) {
        int x = die(r);
        std::println("{}", x);
    }

    std::println();
    std::println("Some normal derivates ...");
    std::normal_distribution normal_dist(0.0, 1.0);

    for (unsigned i = 0; i < 10; ++i) {
        double x = normal_dist(r);
        std::println("{}", x);
    }

    return EXIT_SUCCESS;
}
// ~/~ end
