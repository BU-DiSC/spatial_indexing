#ifndef BENCHMARK_RUNNER_H
#define BENCHMARK_RUNNER_H

#include <string>
#include <spatialindex/SpatialIndex.h>
#include "rtree_helpers.h"
#include "workload_generator.h"
#include "tree_setup.h"

namespace SpatialIndex {

// Run benchmark with given tree and workload generator
void runBenchmark(
    ISpatialIndex* tree,
    WorkloadGenerator& workload_gen,
    int num_insertions,
    const std::string& output_csv
);

} // namespace SpatialIndex

#endif // BENCHMARK_RUNNER_H

