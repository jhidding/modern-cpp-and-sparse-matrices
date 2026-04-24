// ~/~ begin <<docs/eigen.md#src/measurements.hpp>>[init]
#pragma once
#include <vector>
#include <random>
#include "matrix_traits.hpp"

namespace ls_bench {
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
        VectorXd measurement_values(n_measurements);
    
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
}
// ~/~ end
