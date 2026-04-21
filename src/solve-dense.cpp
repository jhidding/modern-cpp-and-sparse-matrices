// ~/~ begin <<docs/eigen.md#src/solve-dense.cpp>>[init]
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

using std::views::enumerate;
using std::views::transform;

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
            = model(measurement_inputs(m, Eigen::all))
            + std::normal_distribution(0.0, noise_level)(r);
    }

    return std::make_tuple(measurement_inputs, measurement_values);
}

int main() {
    unsigned n_coef = 2;
    unsigned c_min = 2, c_max = 3;
    double noise_level = 0.1;
    unsigned n_measurements = 10;

    std::mt19937_64 r;
    auto coef = random_coef(r, n_coef, c_min, c_max);

    std::cout << "Generated coefficients:\n";
    for (auto const [i, c] : coef | enumerate) {
        std::cout << "---- " << i << " --------\n" << c << "\n";
    }

    auto [measurement_inputs, measurement_values]
        = mock_measurements(r, coef, n_measurements, noise_level);
    std::cout << measurement_inputs << "\n";
    std::cout << measurement_values << "\n";

    MatrixXd design_matrix = measurement_inputs.transpose() * measurement_inputs;
    VectorXd design_vector = measurement_inputs.transpose() * measurement_values;
    std::cout << "Design matrix:\n" << design_matrix << "\n";
    std::cout << "Design vector:\n" << design_vector << "\n";

    auto inv_dm = design_matrix.inverse();
    std::cout << "Solution 1:\n" << inv_dm * design_vector << "\n";

    auto solution2 = measurement_inputs.bdcSvd().solve(measurement_values);
    std::cout << "Solution 2:\n" << solution2 << "\n";

    // auto solution = design_matrix.bdcSvd().solve(design_vector);
    // std::cout << "Solution:\n" << solution << "\n";
    return EXIT_SUCCESS;
}
// ~/~ end
