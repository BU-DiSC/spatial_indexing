#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <random>

#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;

void run_benchmark(ISpatialIndex* tree, int num_insertions, const std::string& output_filename) {
    std::vector<long long> insertion_times;
    // store 0 for no split, 1 for split (for plotting)
    std::vector<int> split_events; 

    insertion_times.reserve(num_insertions);
    split_events.reserve(num_insertions);

    // random number generator with float values
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0, 1000.0);

    for (int i = 0; i < num_insertions; ++i) {
        double point_coords[2];
        point_coords[0] = distribution(generator);
        point_coords[1] = distribution(generator);
        Point p(point_coords, 2);

        // to calculate the splits
        // first we get stats before insertion
        IStatistics* stats_before;
        tree->getStatistics(&stats_before);
        uint64_t nodes_before = stats_before->getNumberOfNodes();
        delete stats_before;

        // timing each insertion
        auto start = std::chrono::high_resolution_clock::now();
        tree->insertData(0, nullptr, p, i);
        auto end = std::chrono::high_resolution_clock::now();

        // get stats after insertion and find diff 
        IStatistics* stats_after;
        tree->getStatistics(&stats_after);
        uint64_t nodes_after = stats_after->getNumberOfNodes();
        delete stats_after;
        // if there is diff in num nodes then split has occurred
        bool did_split = (nodes_after > nodes_before);
        split_events.push_back(did_split ? 1 : 0);
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        insertion_times.push_back(duration.count());
    }

    // saving results to csv (1st col=time, 2nd col=node_split)
    // so we can plot graph
    std::ofstream output_file(output_filename);
    output_file << "InsertTime_us,split\n"; // New header
    for (size_t i = 0; i < insertion_times.size(); ++i) {
        output_file << insertion_times[i] << "," << split_events[i] << "\n";
    }
    output_file.close();
}

int main(int argc, char* argv[]) {
    const int NUM_INSERTIONS = 100000;

    // to create rtree:
    // storage manager object, fill factor, index capacity, leaf capacity, dimension, variant, index id
    // num entries (m)= M * fill factor
    // M is max entries (index capacity for both internal and leaf nodes is right now same)
    // the last parameter is output where the function returns id for entries

    // ------- in mem tree --------
    IStorageManager* memory_storage = StorageManager::createNewMemoryStorageManager();
    id_type mem_index_id;
    ISpatialIndex* in_memory_tree = RTree::createNewRTree(*memory_storage, 0.7, 100, 100, 2, RTree::RV_RSTAR, mem_index_id);

    // ------- in disk --------
    std::string disk_tree_filename = "disk_tree";
    remove((disk_tree_filename + ".dat").c_str());
    remove((disk_tree_filename + ".idx").c_str());
    IStorageManager* disk_storage = StorageManager::createNewDiskStorageManager(disk_tree_filename, 4096);
    // RV_STAR, RV_QUADRATIC, RV_LINEAR
    id_type disk_index_id;
    ISpatialIndex* on_disk_tree = RTree::createNewRTree(*disk_storage, 0.7, 100, 100, 2, RTree::RV_RSTAR, disk_index_id);

    run_benchmark(in_memory_tree, NUM_INSERTIONS, "in_memory_insertion_times.csv");
    run_benchmark(on_disk_tree, NUM_INSERTIONS, "on_disk_insertion_times.csv");

    delete in_memory_tree;
    delete memory_storage;
    delete on_disk_tree;
    delete disk_storage;
    return 0;
}
