// ~/~ begin <<docs/benchmark.md#src/benchmark.cpp>>[init]
#include <benchmark/benchmark.h>
#include <random>
#include "measurements.hpp"

using namespace ls_bench;

template <typename M>
static void BM_sparse_qr(benchmark::State &state) {
    // state.PauseTiming();
    std::mt19937_64 r;
    auto coef = random_coef(r, state.range(0), 10, 10);
    auto [input, value] = mock_measurements<M>(r, coef, state.range(1), 0.1);
    // state.ResumeTiming();
    for (auto _ : state) {
        auto result = MatrixTraits<M>::solve_qr(input, value);
        benchmark::DoNotOptimize(result);
    }
}
BENCHMARK(BM_sparse_qr<Sparse>)->Args({10, 1000});
BENCHMARK(BM_sparse_qr<Dense>)->Args({10, 1000});

BENCHMARK_MAIN();
// ~/~ end
