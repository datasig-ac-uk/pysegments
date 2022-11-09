//
// Created by user on 09/11/22.
//


#include <cmath>

#include <benchmark/benchmark.h>

#include <segments.h>


static void bm_single_interval(benchmark::State& state) {
    segments::interval base(0.0, 10.0);
    auto predicate = [](const segments::interval& arg) {
        return 3.14159265358979323846 <= arg.inf() && arg.sup() <= 2*3.1415926535897932384;
    };

    for (auto _ : state) {
        auto result = segments::segment(base, predicate, int(state.range(0)));
        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(1LL<<state.range(0));
}

static void bm_multiple_intervals(benchmark::State& state) {

    segments::interval base(0.0, 10.0);
    auto predicate = [](const segments::interval& arg) {
        return (arg.inf() >= 0.234 && arg.sup() <= 0.9523)
                || (arg.inf() >= 1.042 && arg.sup() <= 1.093)
                || (arg.inf() >= 1.252 && arg.sup() <= 1.301)
                || (arg.inf() >= 1.354 && arg.sup() <= 2.252)
                || (arg.inf() >= 2.852 && arg.sup() <= 3.401)
                || (arg.inf() >= 3.405 && arg.sup() <= 3.509)
                || (arg.inf() >= 3.791 && arg.sup() <= 4.411)
                || (arg.inf() >= 4.925 && arg.sup() <= 5.995)
                || (arg.inf() >= 6.013 && arg.sup() <= 6.521)
                || (arg.inf() >= 6.525 && arg.sup() <= 6.599)
                || (arg.inf() >= 7.354 && arg.sup() <= 8.023)
                || (arg.inf() >= 8.154 && arg.sup() <= 8.832)
                || (arg.inf() >= 9.021 && arg.sup() <= 9.411)
                ;
    };

    for (auto _ : state) {
        auto result = segments::segment(base, predicate, int(state.range(0)));
        benchmark::DoNotOptimize(result.data());
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(1LL<<state.range(0));

}


BENCHMARK(bm_single_interval)->DenseRange(1, 25, 1)->Complexity();
BENCHMARK(bm_multiple_intervals)->DenseRange(1, 25, 1)->Complexity();
