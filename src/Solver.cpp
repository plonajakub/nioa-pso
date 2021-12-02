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
    for (int i = 0; i < mkp_instance.n_objects; ++i) {
        double denominator = 0;
        for (int j = 0; j < mkp_instance.n_dimensions; ++j) {
            denominator += mkp_instance.object_sizes[j][i];
        }
        value_to_size_coeffs[i] = mkp_instance.object_values[i];
        value_to_size_coeffs[i] /= denominator;
    }

    // Sort objects with coefficients
    std::vector<int> obj_idxs_sorted(mkp_instance.n_objects);
    for (int i = 0; i < mkp_instance.n_objects; ++i) {
        obj_idxs_sorted[i] = i;
    }
    std::sort(obj_idxs_sorted.begin(), obj_idxs_sorted.end(), [&](int a, int b) {
        return value_to_size_coeffs[a] > value_to_size_coeffs[b];
    });

    // Construct solution
    MKPSolution solution{std::vector<bool>(mkp_instance.n_objects, false), 0};
    std::vector<int> current_dim_sizes(mkp_instance.n_dimensions);
    bool exit_loop = false;
    for (int i = 0; i < mkp_instance.n_objects; ++i) {
        for (int j = 0; j < mkp_instance.n_dimensions; ++j) {
            if (current_dim_sizes[j] + mkp_instance.object_sizes[j][obj_idxs_sorted[i]] >
                mkp_instance.dimension_capacities[j]) {
                exit_loop = true;
                break;
            }
        }
        if (exit_loop) { break; }
        solution.objects[obj_idxs_sorted[i]] = true;
        solution.total_value += mkp_instance.object_values[obj_idxs_sorted[i]];
        for (int j = 0; j < mkp_instance.n_dimensions; ++j) {
            current_dim_sizes[j] += mkp_instance.object_sizes[j][obj_idxs_sorted[i]];
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
