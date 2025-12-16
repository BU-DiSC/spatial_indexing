#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>

#include "rtree_helpers.h"
#include "workload_generator.h"
#include "tree_setup.h"
#include "benchmark_runner.h"
#include "sware_benchmark.h"

using namespace SpatialIndex;

int main(int argc, char* argv[]) {
    // Expected arguments:
    // 1: <run_type: "mem" or "disk">
    // 2: <M_Capacity>
    // 3: <Fill_Factor>
    // 4: <Num_Insertions>
    // 5: <Buffer_Type: "NONE", "RANDOM", "LRU">
    // 6: <Buffer_Pages> (0 for none)
    // 7: <Tree_Variant: "LINEAR", "RSTAR", "QUADRATIC">
    // 8: <Data_Type: "RANDOM" or "WALK">
    // 9: <Page_Size_Bytes>
    // 10: <Output_CSV_File>

    if (argc != 11) {
        std::cerr << "Error: Invalid number of arguments. Expected 10.\n";
        std::cerr << "Usage: " << argv[0] 
                  << " <run_type> <M> <Fill> <N> <BufferType> <BufferPages> <Variant> <DataType> <PageSize> <OutFile>\n";
        std::cerr << "Example: " << argv[0] 
                  << " disk 16 0.5 100000 LRU 25600 LINEAR RANDOM 4096 output.csv\n";
        return 1; 
    }

    try {
        std::string run_type = argv[1];
        int M = std::stoi(argv[2]);
        double fill_factor = std::stod(argv[3]);
        int num_insertions = std::stoi(argv[4]);
        std::string buffer_type = argv[5];
        int buffer_capacity = std::stoi(argv[6]);
        std::string tree_variant_str = argv[7];
        std::string data_type = argv[8];
        int page_size = std::stoi(argv[9]);
        std::string output_file = argv[10];

        RTree::RTreeVariant tree_variant = getRTreeVariant(tree_variant_str);

        TreeConfig config;
        config.run_type = run_type;
        config.M_capacity = M;
        config.fill_factor = fill_factor;
        config.buffer_type = buffer_type;
        config.buffer_pages = buffer_capacity;
        config.tree_variant = tree_variant;
        config.page_size = page_size;
        config.disk_base_name = "disk_tree_data";

        WorkloadGenerator workload_gen(data_type, 42);

        // auto t_start = std::chrono::high_resolution_clock::now();
        // TreeResources resources = setupTree(config);

        if (config.buffer_type == "SWARE") {
            std::cout << "--- Setting up SWARE Benchmark ---" << std::endl;
            // SWARE must be on-disk to be meaningful
            config.run_type = "disk"; 
            
            // SWARE can use a page buffer (LRU/FIFO) *underneath* it
            // For now, let's keep it simple and use an unbuffered disk
            config.buffer_pages = 0; 
            
            int sware_buffer_items = 10000; // You can make this a config param
            
            TreeResources resources = setupTree(config);
            WorkloadGenerator workload_gen(data_type, 42);

            auto t_start = std::chrono::high_resolution_clock::now();
            run_sware_benchmark(
                resources.tree, 
                workload_gen, 
                num_insertions, 
                sware_buffer_items, // Pass in batch size
                output_file
            );
            auto t_end = std::chrono::high_resolution_clock::now();
            
            auto dur_s = std::chrono::duration_cast<std::chrono::seconds>(t_end - t_start).count();
            std::cout << "Total time for SWARE " << output_file << ": " << dur_s << " seconds.\n\n";

            cleanupTree(resources);

        } else {
            TreeResources resources = setupTree(config);
            // WorkloadGenerator workload_gen(data_type, 42);

            auto t_start = std::chrono::high_resolution_clock::now();
            runBenchmark(resources.tree, workload_gen, num_insertions, output_file);
            auto t_end = std::chrono::high_resolution_clock::now();

            cleanupTree(resources);
            
            auto dur_s = std::chrono::duration_cast<std::chrono::seconds>(t_end - t_start).count();
            std::cout << "Total time for " << output_file << ": " << dur_s << " seconds.\n\n";
        }
        
        // WorkloadGenerator workload_gen(data_type, 42);
        // runBenchmark(resources.tree, workload_gen, num_insertions, output_file);
        // cleanupTree(resources);

        // auto t_end = std::chrono::high_resolution_clock::now();
        // auto dur_s = std::chrono::duration_cast<std::chrono::seconds>(t_end - t_start).count();
        // std::cout << "Total time for " << output_file << ": " << dur_s << " seconds.\n\n";

    } catch (const std::exception& e) {
        std::cerr << "Error during benchmark execution: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Benchmark run completed." << std::endl;
    return 0;
}
