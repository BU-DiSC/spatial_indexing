#pragma once

#include <string>
#include "workload_generator.h"
#include "tree_setup.h"

namespace SpatialIndex {

void run_sware_benchmark(
    ISpatialIndex* tree,
    WorkloadGenerator& workload_gen,
    int num_insertions,
    int items_per_batch,
    const std::string& output_csv
);

} // namespace SpatialIndex
