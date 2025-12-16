#include "workload_generator.h"
#include <random>
#include <algorithm>
#include <cctype>

namespace SpatialIndex {

WorkloadGenerator::WorkloadGenerator(const std::string& data_type, unsigned int seed)
    : data_type_(data_type), gen_(seed), 
      uni_rand_(0.0, 1000.0), 
      walk_dist_(0.0, 5.0),
      initialized_(false) {
    current_coords_[0] = 500.0;
    current_coords_[1] = 500.0;
    initialize();
}

void WorkloadGenerator::initialize() {
    // Convert data_type to uppercase for comparison
    std::string upper_type = data_type_;
    std::transform(upper_type.begin(), upper_type.end(), upper_type.begin(), ::toupper);
    
    if (upper_type != "WALK" && upper_type != "RANDOM") {
        // Default to RANDOM if unknown type
        data_type_ = "RANDOM";
    }
    
    initialized_ = true;
}

void WorkloadGenerator::generateNextPoint(double coords[2]) {
    if (data_type_ == "WALK") {
        // Random walk: update current position with normal distribution step
        current_coords_[0] += walk_dist_(gen_);
        current_coords_[1] += walk_dist_(gen_);
        coords[0] = current_coords_[0];
        coords[1] = current_coords_[1];
    } else {
        // Random uniform distribution
        coords[0] = uni_rand_(gen_);
        coords[1] = uni_rand_(gen_);
    }
}

void WorkloadGenerator::reset() {
    current_coords_[0] = 500.0;
    current_coords_[1] = 500.0;
    gen_.seed(42); // Reset to default seed
}

} // namespace SpatialIndex

