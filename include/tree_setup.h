#ifndef TREE_SETUP_H
#define TREE_SETUP_H

#include <string>
#include <spatialindex/SpatialIndex.h>
#include <spatialindex/RTree.h>

namespace SpatialIndex {

// Structure to hold tree setup configuration
struct TreeConfig {
    std::string run_type;// "mem" or "disk"
    int M_capacity;
    double fill_factor;
    std::string buffer_type;// "NONE", "RANDOM", "LRU"
    int buffer_pages;
    RTree::RTreeVariant tree_variant;
    int page_size;
    std::string disk_base_name; // For disk storage manager
};

// Structure to hold created tree resources
struct TreeResources {
    ISpatialIndex* tree;
    IStorageManager* storage_manager;
    StorageManager::IBuffer* buffer;
    id_type index_id;
    
    TreeResources() : tree(nullptr), storage_manager(nullptr), 
                     buffer(nullptr), index_id(0) {}
};

// Convert string to RTree variant
RTree::RTreeVariant getRTreeVariant(const std::string& variant_str);

// Setup tree based on configuration
TreeResources setupTree(const TreeConfig& config);

// Cleanup tree resources
void cleanupTree(TreeResources& resources);

} // namespace SpatialIndex

#endif // TREE_SETUP_H

