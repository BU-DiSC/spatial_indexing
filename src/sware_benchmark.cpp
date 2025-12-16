#include "sware_benchmark.h"
#include "rtree_helpers.h" 
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm> // For std::sort

namespace SpatialIndex {

struct SwareItem {
    Point p;
    id_type id;
    
    SwareItem() : p(), id(0) {} 
    
    SwareItem(const Point& pt, id_type i) : p(pt), id(i) {}

    bool operator<(const SwareItem& other) const {
        return p.m_pCoords[0] < other.p.m_pCoords[0];
    }
};

void run_sware_benchmark(
    ISpatialIndex* tree,
    WorkloadGenerator& workload_gen,
    int num_insertions,
    int items_per_batch,
    const std::string& output_csv
) {
    std::ofstream f(output_csv);
    if (!f.is_open()) {
        std::cerr << "Error: Could not open SWARE output file: " << output_csv << std::endl;
        return;
    }
    
    std::cout << "Starting SWARE benchmark: " << num_insertions << " insertions (" 
              << workload_gen.getDataType() << " data) -> " << output_csv << std::endl;
    std::cout << "  Batch size: " << items_per_batch << " items" << std::endl;
    
    // Log batch stats, not per-item stats
    f << "BatchIdx,ItemsInBatch,Time_us,HeightBefore,HeightAfter,SplitsBefore,SplitsAfter\n";

    // This is our object buffer
    std::vector<SwareItem> buffer;
    buffer.reserve(items_per_batch);
    
    int batch_index = 0;
    
    for (int i = 0; i < num_insertions; ++i) {
        // 1. Generate point
        double coords[2];
        workload_gen.generateNextPoint(coords);
        
        // 2. Add to buffer
        buffer.emplace_back(SwareItem(Point(coords, 2), static_cast<id_type>(i)));
        
        // 3. If buffer is full (or it's the very last insertion), flush the batch
        if (buffer.size() == items_per_batch || i == num_insertions - 1) {
            
            // Get stats before the batch
            const uint32_t h_before  = rtree_height(*tree);
            const uint64_t sp_before = rtree_splits(*tree);
            
            // --- This is the SWARE/Bulk-Load idea ---
            // 3a. Sort the buffer (e.g., by X-coordinate)
            auto sort_start = std::chrono::high_resolution_clock::now();
            std::sort(buffer.begin(), buffer.end());
            auto sort_end = std::chrono::high_resolution_clock::now();
            
            // 3b. Insert the sorted batch
            auto insert_start = std::chrono::high_resolution_clock::now();
            for (const auto& item : buffer) {
                tree->insertData(0, nullptr, item.p, item.id);
            }
            auto insert_end = std::chrono::high_resolution_clock::now();
            // ------------------------------------------

            // Get stats after the batch
            const uint32_t h_after  = rtree_height(*tree);
            const uint64_t sp_after = rtree_splits(*tree);
            
            auto sort_dur_us = std::chrono::duration_cast<std::chrono::microseconds>(sort_end - sort_start).count();
            auto insert_dur_us = std::chrono::duration_cast<std::chrono::microseconds>(insert_end - insert_start).count();
            auto total_dur_us = sort_dur_us + insert_dur_us;

            // Log stats for the whole batch
            f << batch_index << "," << buffer.size() << "," << total_dur_us << ","
              << h_before << "," << h_after << ","
              << sp_before << "," << sp_after << "\n";
              
            if (batch_index % 10 == 0) {
                 std::cout << "  ... Flushed batch " << batch_index 
                           << " (" << i + 1 << "/" << num_insertions << " items)"
                           << " in " << total_dur_us << " us (" 
                           << sort_dur_us << " us sorting, " 
                           << insert_dur_us << " us inserting)\n";
            }
            
            buffer.clear();
            batch_index++;
        }
    }
    
    f.close();
    std::cout << "SWARE benchmark finished for " << output_csv << "." << std::endl;
}

} // namespace SpatialIndex
