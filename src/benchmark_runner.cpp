#include "benchmark_runner.h"
#include <fstream>
#include <iostream>
#include <chrono>

namespace SpatialIndex {

void runBenchmark(
    ISpatialIndex* tree,
    WorkloadGenerator& workload_gen,
    int num_insertions,
    const std::string& output_csv
) {
    std::ofstream f(output_csv);
    if (!f.is_open()) {
        std::cerr << "Error: Could not open output file: " << output_csv << std::endl;
        return;
    }
    
    std::cout << "Starting benchmark: " << num_insertions << " insertions (" 
              << workload_gen.getDataType() << " data) -> " << output_csv << std::endl;
    
    // Write CSV header
    f << "InsertIdx,Time_us,DidSplit,IsRootSplit,NodesBefore,NodesAfter,"
         "HeightBefore,HeightAfter,SplitsBefore,SplitsAfter\n";
    
    int progress_milestone = num_insertions / 10;
    if (progress_milestone == 0) progress_milestone = 1;
    
    for (int i = 0; i < num_insertions; ++i) {
        // Get statistics before insertion
        const uint32_t nodes_before = rtree_nodes(*tree);
        const uint32_t h_before = rtree_height(*tree);
        const uint64_t sp_before = rtree_splits(*tree);
        
        // Generate point coordinates and measure insertion time
        double coords[2];
        workload_gen.generateNextPoint(coords);
        Point p(coords, 2);
        auto t0 = std::chrono::high_resolution_clock::now();
        tree->insertData(0, nullptr, p, static_cast<id_type>(i));
        auto t1 = std::chrono::high_resolution_clock::now();
        
        // Get statistics after insertion
        const uint32_t nodes_after = rtree_nodes(*tree);
        uint32_t h_after = h_before;
        uint64_t sp_after = sp_before;
        
        // Only query height and splits if node count changed (optimization)
        if (nodes_after > nodes_before) {
            h_after = rtree_height(*tree);
            sp_after = rtree_splits(*tree);
        }
        
        const bool did_split = (nodes_after > nodes_before) || (sp_after > sp_before);
        const bool root_split = (h_after > h_before);
        const auto dur_us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        
        // Write to CSV
        f << i << "," << dur_us << "," << (did_split ? 1 : 0) << "," 
          << (root_split ? 1 : 0) << ","
          << nodes_before << "," << nodes_after << ","
          << h_before << "," << h_after << ","
          << sp_before << "," << sp_after << "\n";
        
        // Progress reporting
        if ((i + 1) % progress_milestone == 0) {
            int percentage = static_cast<int>(((i + 1) * 100) / num_insertions);
            std::cout << "  ... Progress for " << output_csv << ": "
                      << percentage << "% completed ("
                      << (i + 1) << " insertions)\n";
        }
    }
    
    f.close();
    std::cout << "Benchmark finished for " << output_csv << "." << std::endl;
}

} // namespace SpatialIndex

