#include "Solver.h"
#include <vector>
#include <algorithm>

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

#include "mkp.h"

MKPSolution Solver::greedy(const MKPInstance &mkp_instance, const SolverSettings &settings) {
    // Calculate coefficients
    std::vector<double> value_to_size_coeffs(mkp_instance.n_objects);
    for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
        double denominator = 0;
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            denominator += mkp_instance.object_sizes[dim_i][obj_i];
        }
        value_to_size_coeffs[obj_i] = mkp_instance.object_values[obj_i];
        value_to_size_coeffs[obj_i] /= denominator;
    }

    // Sort objects with coefficients
    std::vector<int> obj_idxs_sorted(mkp_instance.n_objects);
    for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
        obj_idxs_sorted[obj_i] = obj_i;
    }
    std::sort(obj_idxs_sorted.begin(), obj_idxs_sorted.end(), [&](int a, int b) {
        return value_to_size_coeffs[a] > value_to_size_coeffs[b];
    });

    // Construct solution
    MKPSolution solution{std::vector<bool>(mkp_instance.n_objects, false), 0};
    std::vector<int> current_dim_sizes(mkp_instance.n_dimensions);
    for (int obj_i : obj_idxs_sorted) {
        bool continue_loop = false;
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            if (current_dim_sizes[dim_i] + mkp_instance.object_sizes[dim_i][obj_i] >
                mkp_instance.dimension_capacities[dim_i]) {
                continue_loop = true;
                break;
            }
        }
        if (continue_loop) { continue; }
        solution.objects[obj_i] = true;
        solution.total_value += mkp_instance.object_values[obj_i];
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            current_dim_sizes[dim_i] += mkp_instance.object_sizes[dim_i][obj_i];
        }
    }
    return solution;
}

MKPSolution Solver::particle_swarm(const MKPInstance &mkp_instance, const SolverSettings &settings) {
    std::this_thread::sleep_for(
            std::chrono::milliseconds(50 + static_cast<int>(static_cast<double>(rand()) / RAND_MAX * 50)));
    return {std::vector<bool>(mkp_instance.n_objects, false),
            10000 + static_cast<int>(static_cast<double>(rand()) / RAND_MAX * 1000)};
}
