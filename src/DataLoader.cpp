#include "DataLoader.h"
#include <fstream>

void ignore_lines(std::ifstream &input, int n_lines) {
    for (int i = 0; i < n_lines; ++i) {
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::vector<MKPInstance> DataLoader::load_mknap2(const std::string &path) {
    std::ifstream ifs(path);
    std::vector<MKPInstance> mknap2_instances;

    ignore_lines(ifs, 85);
    MKPInstance mkp_instance;
    int id = 0;
    while (ifs >> mkp_instance.n_dimensions >> mkp_instance.n_objects) {
        mkp_instance.id = id;
        for (int i = 0; i < mkp_instance.n_objects; ++i) {
            int object_value;
            ifs >> object_value;
            mkp_instance.object_values.push_back(object_value);
        }
        for (int i = 0; i < mkp_instance.n_dimensions; ++i) {
            int dimension_capacity;
            ifs >> dimension_capacity;
            mkp_instance.dimension_capacities.push_back(dimension_capacity);
        }
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            std::vector<int> dim_object_sizes;
            for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
                int object_size;
                ifs >> object_size;
                dim_object_sizes.push_back(object_size);
            }
            mkp_instance.object_sizes.push_back(dim_object_sizes);
        }
        ifs >> mkp_instance.known_optimum;
        mknap2_instances.push_back(mkp_instance);

        ignore_lines(ifs, 7);
        ++id;
        mkp_instance = MKPInstance();
    }
    return mknap2_instances;
}
