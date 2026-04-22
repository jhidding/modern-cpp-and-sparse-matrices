---
icon: material/owl
---

# About Eigen

Eigen is a header-only library for doing linear algebra in C++.

```meson
#| id: meson-executables
executable('eigen-hello', 'src/eigen_hello.cpp',
    dependencies: [libeigen])
```

```c++
//| file: src/eigen_hello.cpp
#include <cstdlib>
#include <iostream>
#include <Eigen/Dense>

using Eigen::MatrixXd;

int main() {
    MatrixXd m(2,2);
    m(0,0) = 3;
    m(1,0) = 2.5;
    m(0,1) = -1;
    m(1,1) = m(1,0) + m(0,1);
    std::cout << m << "\n";
    return EXIT_SUCCESS;
}
```

## Random Numbers

C++ has a nice library for generating random numbers.

```meson
#| id: meson-executables
executable('random-values', 'src/random_values.cpp')
```

```c++
//| file: src/random_values.cpp
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
```

## Linear Algebra

```meson
#| id: meson-executables
executable('least-squares', 'src/least-squares.cpp',
    dependencies: [libeigen, argparse, benchmark],
    include_directories: incdir)
```

Now we get to do some linear algebra on randomly generated block diagonal matrices. We suppose some arbitrary linear model with $n$ normally distributed coefficients $c_i$. We then simulate $m$ measurements of this model and add a little noise, resulting in a linear system of size $n \times m$, which we can solve using one of several available least-squares methods in Eigen.

### Matrix traits

We'll have a little system of trait classes to write code that is generic for both dense and sparse matrices. To make sparse matrices perform efficiently, it can be advantageous to know the number of non-zero elements before hand. For now, we will only look at the efficiency of solving the actual linear system.

```c++
//| id: matrix-traits
struct Sparse {};
struct Dense {};

template <typename T>
struct MatrixTraits {};
```

After a sparse array is created, we need to compress the array, to make the QR solver work. Roughly what this does, it orders elements by column giving more efficient traversal. For dense matrices, this method is left empty. The QR decomposition is stored in the solver instance. If we need to solve for multiple vectors it is most efficient to keep the solver objects alive.

```c++
//| file: src/matrix_traits.hpp
#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseQR>

namespace ls_bench {
    using Eigen::MatrixXd;
    using Eigen::VectorXd;

    <<matrix-traits>>
}
```

### Dense matrices

```c++
//| id: matrix-traits
template <>
struct MatrixTraits<Dense> {
    typedef Eigen::MatrixXd MatrixType;

    inline static void set_element(MatrixType &A, unsigned i, unsigned j, double value) {
        A(i, j) = value;
    }

    inline static void make_compressed(MatrixType &A) {}

    inline static VectorXd solve_qr(MatrixType const &A, VectorXd const &b) {
        Eigen::HouseholderQR<MatrixXd> direct_solver_qr(A);
        return direct_solver_qr.solve(b);
    }
};
```

### Sparse matrices

```c++
//| id: matrix-traits
template <>
struct MatrixTraits<Sparse> {
    typedef Eigen::SparseMatrix<double> MatrixType;

    inline static void set_element(MatrixType &A, unsigned i, unsigned j, double value) {
        A.insert(i, j) = value;
    }

    inline static void make_compressed(MatrixType &A) {
        A.makeCompressed();
    }

    inline static VectorXd solve_qr(MatrixType const &A, VectorXd const &b) {
        Eigen::SparseQR<MatrixType, Eigen::COLAMDOrdering<int>> direct_solver_qr(A);
        return direct_solver_qr.solve(b);
    }
};
```

### Mocking measurements

We emulate measurements involving only small parts of the full model domain. This results in block-diagonal design matrices. We could sprinkle around cross-terms later on. We define the multi-linear model by generating a set of vectors of random sizes.

```c++
//| id: mock-measurements
template <typename RNG>
std::vector<VectorXd> random_coef(RNG &r, unsigned n, unsigned a, unsigned b) {
    std::vector<VectorXd> result;
    std::uniform_int_distribution<unsigned> block_size_dist(a, b);
    std::normal_distribution normal_dist(0.0, 1.0);

    for (unsigned i = 0; i < n; ++i) {
        size_t block_size = block_size_dist(r);
        result.emplace_back(block_size);
        for (auto &c : result.back()) {
            c = normal_dist(r);
        }
    }

    return result;
}
```

From this model we generate our measurements, returning a rectangular matrix and a vector.

```c++
//| id: mock-measurements
template <typename M, typename RNG>
std::tuple<typename MatrixTraits<M>::MatrixType, VectorXd> mock_measurements(
        RNG &r, std::vector<VectorXd> const &coef, size_t n_measurements, double noise_level) {

    using Matrix = MatrixTraits<M>::MatrixType;

    size_t n_coef = coef.size();
    std::vector<size_t> coef_idx;
    std::exclusive_scan(coef.cbegin(), coef.cend(), std::back_inserter(coef_idx), 0,
        [] (size_t x, auto const &c) { return x + c.size(); });

    size_t total_domain_size = coef_idx.back() + coef.back().size();
    Matrix measurement_inputs(n_measurements, total_domain_size);
    Eigen::VectorXd measurement_values(n_measurements);

    for (size_t m = 0; m < n_measurements; ++m) {
        unsigned c = std::uniform_int_distribution<unsigned>(0, n_coef-1)(r);
        double y = 0.0;
        for (size_t i = 0; i < (size_t)coef[c].size(); ++i) {
            double x = std::normal_distribution(0.0, 1.0)(r);
            MatrixTraits<M>::set_element(measurement_inputs, m, coef_idx[c] + i, x);
            y += coef[c][i] * x;
        }

        measurement_values(m) = y + std::normal_distribution(0.0, noise_level)(r);
    }

    MatrixTraits<M>::make_compressed(measurement_inputs);
    return std::make_tuple(measurement_inputs, measurement_values);
}
```

### Running the experiment

```c++
//| id: run-experiment
template <typename M, typename RNG>
void run_experiment(RNG &r, std::vector<VectorXd> const &coef, unsigned n_measurements, double noise_level) {
    auto [measurement_inputs, measurement_values]
        = mock_measurements<M>(r, coef, n_measurements, noise_level);

    auto result = MatrixTraits<M>::solve_qr(measurement_inputs, measurement_values);
    std::cout << "Solution:\n" << result << std::endl;
}
```

### Main

We use the [Argparse library](https://github.com/p-ranav/argparse) to do some argument parsing.

??? "Argument parsing"

    ```c++
    //| id: argument-parsing
    argparse::ArgumentParser program("least-squares");

    program.add_argument("-n")
        .help("number of coefficient chunks")
        .store_into(n_coef);

    program.add_argument("-cmin")
        .help("minimum coefficient chunk size")
        .store_into(c_min);

    program.add_argument("-cmax")
        .help("maximum coefficient chunk size")
        .store_into(c_max);

    program.add_argument("-s", "--noise-level")
        .help("noise amplitude")
        .store_into(noise_level);

    program.add_argument("-m")
        .help("number of measurements")
        .store_into(n_measurements);

    auto &matrix_type = program.add_mutually_exclusive_group(true);
    matrix_type.add_argument("-sparse")
        .help("use sparse matrices")
        .flag();
    matrix_type.add_argument("-dense")
        .help("use dense matrices")
        .flag();

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &exc) {
        std::cerr << exc.what() << std::endl;
        std::cerr << program;
        std::exit(EXIT_FAILURE);
    }
    ```

In the main function, we define the parameters with their default values, parse arguments, generate mock data and then run the experiment.

```c++
//| id: least-squares-main
unsigned n_coef = 5;
unsigned c_min = 3;
unsigned c_max = 8;
double noise_level = 0.1;
unsigned n_measurements = 100;

<<argument-parsing>>

std::mt19937_64 r;
auto coef = random_coef(r, n_coef, c_min, c_max);

std::cout << "Generated coefficients:\n";
for (auto const [i, c] : coef | enumerate) {
    std::cout << "---- " << i << " --------\n" << c << "\n";
}

if (program["-dense"] == true) {
    run_experiment<Dense>(r, coef, n_measurements, noise_level);
} else {
    run_experiment<Sparse>(r, coef, n_measurements, noise_level);
}

return EXIT_SUCCESS;
```

??? "file: src/least-squares.cpp"

    ```c++
    //| file: src/least-squares.cpp
    #include <argparse/argparse.hpp>
    #include <cstdlib>
    #include <iostream>
    #include <Eigen/Dense>
    #include <Eigen/Sparse>
    #include <Eigen/SparseQR>
    #include <random>
    #include <functional>
    #include <numeric>
    #include <ranges>

    #include "matrix_traits.hpp"

    using namespace ls_bench;

    using Eigen::MatrixXd;
    using Eigen::VectorXd;
    using Eigen::Vector3d;
    using Eigen::seqN;
    using DenseMatrix = Eigen::MatrixXd;
    using SparseMatrix = Eigen::SparseMatrix<double>;
    using std::views::enumerate;

    <<mock-measurements>>
    <<run-experiment>>

    std::function<double(VectorXd const &)> multi_linear_function(std::vector<VectorXd> const &coef) {
        return [&coef] (VectorXd const &x) {
            size_t idx = 0;
            double result = 0.0;
            for (auto &c : coef) {
                result += c.dot(x(seqN(idx, c.size())));
                idx += c.size();
            }
            return result;
        };
    }

    int main(int argc, char *argv[]) {
        <<least-squares-main>>
    }
    ```

```c++
// Matrix design_matrix = measurement_inputs.transpose() * measurement_inputs;
// VectorXd design_vector = measurement_inputs.transpose() * measurement_values;

// Eigen::ColPivHouseholderQR<MatrixXd> solver(design_matrix);
// auto solution3 = solver.solve(design_vector);
// std::cout << "Solution solving design matrix:\n" << solution3 << "\n";

// Eigen::BDCSVD<MatrixXd, Eigen::ComputeThinU | Eigen::ComputeThinV> direct_solver(measurement_inputs);
// auto solution4 = direct_solver.solve(measurement_values);
// std::cout << "Solution BDC SVD:\n" << solution4 << "\n";

// Eigen::HouseholderQR<MatrixXd> direct_solver_qr(measurement_inputs);
// auto solution2 = direct_solver_qr.solve(measurement_values);
// std::cout << "Solution HouseholderQR:\n" << solution2 << "\n";

// auto inv_dm = design_matrix.inverse();
// std::cout << "Solution using design matrix inverse:\n" << inv_dm * design_vector << "\n";
```
