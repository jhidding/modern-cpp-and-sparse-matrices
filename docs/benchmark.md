---
title: Benchmarking
---

```meson
#| id: meson-executables
executable('ls-bench', 'src/benchmark.cpp',
    dependencies: [libeigen, benchmark],
    include_directories: incdir)
```

We use Google's `benchmark` library to setup some benchmarks for both run-time and memory use.

```c++
//| file: src/benchmark.cpp
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
```

## Memory

The `benchmark` library can tell us a lot about compute efficiency, but it doesn't naturally measure memory usage. There is a utility in `benchmark` to measure memory use, but it requires creating a custom allocator and then forcing all of the code to use that allocator. An easier method is to use Valgrind and its `massif` tool to track memory usage from the outside. Simply run:

```bash
valgrid --tool=massif ./build/debug/least-squares -n 10 -m 100000 -dense
```

to measure the memory use of the `least-squares` test program.
