#ifndef WORKLOAD_GENERATOR_H
#define WORKLOAD_GENERATOR_H

#include <string>
#include <random>
#include <spatialindex/SpatialIndex.h>

namespace SpatialIndex {

// Workload generator class for generating points based on distribution type
class WorkloadGenerator {
public:
    WorkloadGenerator(const std::string& data_type, unsigned int seed = 42);
    
    // Generate the next point coordinates (fills coords array)
    void generateNextPoint(double coords[2]);
    
    // Reset the generator state
    void reset();
    
    // Get the distribution type
    std::string getDataType() const { return data_type_; }

private:
    std::string data_type_;
    std::mt19937 gen_;
    std::uniform_real_distribution<double> uni_rand_;
    std::normal_distribution<double> walk_dist_;
    double current_coords_[2];
    bool initialized_;
    
    void initialize();
};

} // namespace SpatialIndex

#endif // WORKLOAD_GENERATOR_H

