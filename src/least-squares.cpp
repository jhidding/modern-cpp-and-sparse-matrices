// ~/~ begin <<docs/eigen.md#src/least-squares.cpp>>[init]
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

// ~/~ begin <<docs/eigen.md#mock-measurements>>[init]
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
// ~/~ end
// ~/~ begin <<docs/eigen.md#mock-measurements>>[1]
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
// ~/~ end
// ~/~ begin <<docs/eigen.md#run-experiment>>[init]
template <typename M, typename RNG>
void run_experiment(RNG &r, std::vector<VectorXd> const &coef, unsigned n_measurements, double noise_level) {
    auto [measurement_inputs, measurement_values]
        = mock_measurements<M>(r, coef, n_measurements, noise_level);

    auto result = MatrixTraits<M>::solve_qr(measurement_inputs, measurement_values);
    std::cout << "Solution:\n" << result << std::endl;
}
// ~/~ end

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
    // ~/~ begin <<docs/eigen.md#least-squares-main>>[init]
    unsigned n_coef = 5;
    unsigned c_min = 3;
    unsigned c_max = 8;
    double noise_level = 0.1;
    unsigned n_measurements = 100;
    
    // ~/~ begin <<docs/eigen.md#argument-parsing>>[init]
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
    // ~/~ end
    
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
    // ~/~ end
}
// ~/~ end
