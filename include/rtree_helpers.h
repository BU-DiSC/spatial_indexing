#ifndef RTREE_HELPERS_H
#define RTREE_HELPERS_H

#include <spatialindex/SpatialIndex.h>
#include <spatialindex/rtree/IRTreeStatistics.h>
#include <cstdint>

namespace SpatialIndex {

// Helper functions to get RTree statistics
inline uint32_t rtree_height(ISpatialIndex& idx) {
    IStatistics* base = nullptr;
    idx.getStatistics(&base);
    uint32_t h = 0;
    if (auto* r = dynamic_cast<const RTree::IRTreeStatistics*>(base)) {
        h = r->getTreeHeight();
    }
    delete base;
    return h;
}

inline uint64_t rtree_splits(ISpatialIndex& idx) {
    IStatistics* base = nullptr;
    idx.getStatistics(&base);
    uint64_t s = 0;
    if (auto* r = dynamic_cast<const RTree::IRTreeStatistics*>(base)) {
        s = r->getSplits();
    }
    delete base;
    return s;
}

inline uint32_t rtree_nodes(ISpatialIndex& idx) {
    IStatistics* base = nullptr;
    idx.getStatistics(&base);
    uint32_t n = base ? base->getNumberOfNodes() : 0;
    delete base;
    return n;
}

} // namespace SpatialIndex

#endif // RTREE_HELPERS_H

