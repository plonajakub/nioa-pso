#ifndef NIOA_PSO_MKP_H
#define NIOA_PSO_MKP_H

#include <vector>

struct MKPInstance {
    int id;
    int n_dimensions; // or n_knapsacks
    int n_objects;
    std::vector<int> object_values;
    std::vector<int> dimension_capacities;
    std::vector<std::vector<int>> object_sizes;
    int known_optimum;
};

struct MKPSolution {
    std::vector<bool> objects;
    int total_value;
};


#endif //NIOA_PSO_MKP_H
