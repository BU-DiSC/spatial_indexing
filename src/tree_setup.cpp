// src/tree_setup.cpp
#include "tree_setup.h"
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <stdexcept>

namespace SpatialIndex {

RTree::RTreeVariant getRTreeVariant(const std::string& variant_str) {
    std::string upper = variant_str;
    std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);
    
    if (upper == "RSTAR") return RTree::RV_RSTAR;
    if (upper == "QUADRATIC") return RTree::RV_QUADRATIC;
    return RTree::RV_LINEAR; // Default
}


TreeResources setupTree(const TreeConfig& config) {
    TreeResources resources;
    
    std::string run_type_upper = config.run_type;
    std::transform(run_type_upper.begin(), run_type_upper.end(), 
                   run_type_upper.begin(), ::toupper);
    
    if (run_type_upper == "MEM") {
        std::cout << "--- Setting up In-Memory Tree ---" << std::endl;
        resources.storage_manager = StorageManager::createNewMemoryStorageManager();
        resources.tree = RTree::createNewRTree(
            *resources.storage_manager,
            config.fill_factor,
            config.M_capacity,
            config.M_capacity,
            2, // dimension
            config.tree_variant,
            resources.index_id
        );
        
    } else if (run_type_upper == "DISK") {
        std::cout << "--- Setting up On-Disk Tree ---" << std::endl;
        
        std::string base_name = config.disk_base_name.empty() ? 
                                "disk_tree_data" : config.disk_base_name;
        std::remove((base_name + ".dat").c_str());
        std::remove((base_name + ".idx").c_str());
        
        resources.storage_manager = StorageManager::createNewDiskStorageManager(
            base_name, 
            config.page_size
        );
        
        // Setup buffer if needed
        std::string buffer_type_upper = config.buffer_type;
        std::transform(buffer_type_upper.begin(), buffer_type_upper.end(),
                      buffer_type_upper.begin(), ::toupper);
        
        if (buffer_type_upper == "RANDOM" && config.buffer_pages > 0) {
            std::cout << "  Using RANDOM Evictions Buffer with capacity: " 
                      << config.buffer_pages << " pages." << std::endl;
            resources.buffer = StorageManager::createNewRandomEvictionsBuffer(
                *resources.storage_manager, config.buffer_pages, false
            );
            resources.tree = RTree::createNewRTree(
                *resources.buffer, config.fill_factor, config.M_capacity,
                config.M_capacity, 2, config.tree_variant, resources.index_id
            );
        } else if (buffer_type_upper == "FIFO" && config.buffer_pages > 0) {
            std::cout << "  Using FIFO Evictions Buffer with capacity: " 
                      << config.buffer_pages << " pages." << std::endl;
            // resources.buffer = StorageManager::createNewFIFOEvictionsBuffer(
            //     *resources.storage_manager, config.buffer_pages, false
            // );
            resources.buffer = SpatialIndex::StorageManager::createNewFIFOEvictionsBuffer(
                *resources.storage_manager, config.buffer_pages, false
            );
            resources.tree = RTree::createNewRTree(
                *resources.buffer, config.fill_factor, config.M_capacity,
                config.M_capacity, 2, config.tree_variant, resources.index_id
            );
        // ---------------------

        } else if (buffer_type_upper == "LRU" && config.buffer_pages > 0) {
            std::cout << "  Using LRU Evictions Buffer with capacity: " 
                      << config.buffer_pages << " pages." << std::endl;
            // resources.buffer = StorageManager::createNewLRUEvictionsBuffer(
            //     *resources.storage_manager, config.buffer_pages, false
            // );
            resources.buffer = SpatialIndex::StorageManager::createNewLRUEvictionsBuffer(
                *resources.storage_manager, config.buffer_pages, false
            );
            resources.tree = RTree::createNewRTree(
                *resources.buffer, config.fill_factor, config.M_capacity,
                config.M_capacity, 2, config.tree_variant, resources.index_id
            );
        } else {
            std::cout << "  Using NO Buffer (Raw Disk I/O)." << std::endl;
            resources.tree = RTree::createNewRTree(
                *resources.storage_manager, config.fill_factor, config.M_capacity,
                config.M_capacity, 2, config.tree_variant, resources.index_id
            );
        }
    } else {
        throw std::runtime_error("Unknown run_type: " + config.run_type + 
                                ". Use 'mem' or 'disk'.");
    }
    
    return resources;
}

// ... (cleanupTree function is unchanged) ...
void cleanupTree(TreeResources& resources) {
    if (resources.tree) {
        resources.tree->flush();
        delete resources.tree;
        resources.tree = nullptr;
    }
    if (resources.buffer) {
        delete resources.buffer;
        resources.buffer = nullptr;
    }
    if (resources.storage_manager) {
        delete resources.storage_manager;
        resources.storage_manager = nullptr;
    }
}

} // namespace SpatialIndex
