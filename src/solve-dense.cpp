// ~/~ begin <<docs/eigen.md#src/solve-dense.cpp>>[init]
#include <argparse/argparse.hpp>
#include <cstdlib>
#include <iostream>
#include <Eigen/Dense>
#include <random>
#include <functional>
#include <numeric>
#include <ranges>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::Vector3d;
using Eigen::seqN;
using Eigen::placeholders::all;
using std::views::enumerate;

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

template <typename RNG>
std::tuple<MatrixXd, VectorXd> mock_measurements(
        RNG &r,
        std::vector<VectorXd> const &coef,
        size_t n_measurements,
        double noise_level) {
    size_t n_coef = coef.size();
    std::vector<size_t> coef_idx;
    std::exclusive_scan(coef.cbegin(), coef.cend(), std::back_inserter(coef_idx), 0,
        [] (size_t x, auto const &c) { return x + c.size(); });

    size_t total_domain_size = coef_idx.back() + coef.back().size();
    Eigen::MatrixXd measurement_inputs(n_measurements, total_domain_size);
    Eigen::VectorXd measurement_values(n_measurements);

    auto model = multi_linear_function(coef);
    for (size_t m = 0; m < n_measurements; ++m) {
        unsigned c = std::uniform_int_distribution<unsigned>(0, n_coef-1)(r);
        for (size_t i = 0; i < (size_t)coef[c].size(); ++i) {
            measurement_inputs(m, coef_idx[c] + i) = std::normal_distribution(0.0, 1.0)(r);
        }

        measurement_values(m)
            = model(measurement_inputs(m, all))
            + std::normal_distribution(0.0, noise_level)(r);
    }

    return std::make_tuple(measurement_inputs, measurement_values);
}

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("least-squares");
    unsigned n_coef = 5;
    program.add_argument("-n")
        .help("number of coefficient chunks")
        .store_into(n_coef);

    unsigned c_min = 3;
    program.add_argument("-cmin")
        .help("minimum coefficient chunk size")
        .store_into(c_min);

    unsigned c_max = 8;
    program.add_argument("-cmax")
        .help("maximum coefficient chunk size")
        .store_into(c_max);

    double noise_level = 0.1;
    program.add_argument("-s", "--noise-level")
        .help("noise amplitude")
        .store_into(noise_level);

    unsigned n_measurements = 100;
    program.add_argument("-m")
        .help("number of measurements")
        .store_into(n_measurements);

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::exception &exc) {
        std::cerr << exc.what() << std::endl;
        std::cerr << program;
        std::exit(EXIT_FAILURE);
    }

    std::mt19937_64 r;
    auto coef = random_coef(r, n_coef, c_min, c_max);

    std::cout << "Generated coefficients:\n";
    for (auto const [i, c] : coef | enumerate) {
        std::cout << "---- " << i << " --------\n" << c << "\n";
    }

    auto [measurement_inputs, measurement_values]
        = mock_measurements(r, coef, n_measurements, noise_level);

    MatrixXd design_matrix = measurement_inputs.transpose() * measurement_inputs;
    VectorXd design_vector = measurement_inputs.transpose() * measurement_values;

    auto inv_dm = design_matrix.inverse();
    std::cout << "Solution using design matrix inverse:\n" << inv_dm * design_vector << "\n";

    Eigen::ColPivHouseholderQR<MatrixXd> solver(design_matrix);
    auto solution3 = solver.solve(design_vector);
    std::cout << "Solution solving design matrix:\n" << solution3 << "\n";

    Eigen::BDCSVD<MatrixXd, Eigen::ComputeThinU | Eigen::ComputeThinV> direct_solver(measurement_inputs);
    auto solution4 = direct_solver.solve(measurement_values);
    std::cout << "Solution BDC SVD:\n" << solution4 << "\n";

    Eigen::HouseholderQR<MatrixXd> direct_solver_qr(measurement_inputs);
    auto solution2 = direct_solver_qr.solve(measurement_values);
    std::cout << "Solution HouseholderQR:\n" << solution2 << "\n";

    return EXIT_SUCCESS;
}
// ~/~ end
