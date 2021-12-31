#include "Solver.h"
#include <vector>
#include <algorithm>
#include <cassert>

#include "mkp.h"
#include "Random.h"
#include "Utils.h"

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
    MKPSolution solution{std::vector<bool>(mkp_instance.n_objects, false), std::vector<int>(mkp_instance.n_dimensions),
                         0};
    for (int obj_i: obj_idxs_sorted) {
        bool continue_loop = false;
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            if (solution.dimension_sizes[dim_i] + mkp_instance.object_sizes[dim_i][obj_i] >
                mkp_instance.dimension_capacities[dim_i]) {
                continue_loop = true;
                break;
            }
        }
        if (continue_loop) { continue; }
        solution.objects[obj_i] = true;
        solution.total_value += mkp_instance.object_values[obj_i];
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            solution.dimension_sizes[dim_i] += mkp_instance.object_sizes[dim_i][obj_i];
        }
    }
    return solution;
}

MKPSolution Solver::particle_swarm(const MKPInstance &mkp_instance, const SolverSettings &settings) {
    // Initialize object priority list for repair operator
    std::vector<double> value_to_size_coeffs(mkp_instance.n_objects);
    for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
        double denominator = 0;
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            denominator += mkp_instance.object_sizes[dim_i][obj_i];
        }
        value_to_size_coeffs[obj_i] = mkp_instance.object_values[obj_i] / denominator;
    }
    // Descending priority (most important object first)
    std::vector<int> obj_priority_list(mkp_instance.n_objects);
    for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
        obj_priority_list[obj_i] = obj_i;
    }
    std::sort(obj_priority_list.begin(), obj_priority_list.end(), [&](int a, int b) {
        return value_to_size_coeffs[a] > value_to_size_coeffs[b];
    });

    std::vector<MKPSolution> particles(settings.n_particles);
    for (auto &particle: particles) {
        particle.objects = std::vector<bool>(mkp_instance.n_objects, false);
        particle.dimension_sizes = std::vector<int>(mkp_instance.n_dimensions, 0);
        particle.total_value = 0;
        for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
            bool continue_loop = false;
            for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
                if (particle.dimension_sizes[dim_i] + mkp_instance.object_sizes[dim_i][obj_i] >
                    mkp_instance.dimension_capacities[dim_i]) {
                    continue_loop = true;
                    break;
                }
            }
            if (continue_loop) { continue; }
            if (Random::getRealClosed(0, 1) < 0.5) {
                particle.objects[obj_i] = true;
                particle.total_value += mkp_instance.object_values[obj_i];
                for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
                    particle.dimension_sizes[dim_i] += mkp_instance.object_sizes[dim_i][obj_i];
                }
            }
        }
        assert(check_solution(particle, mkp_instance, false));
    }

    std::vector<std::vector<double>> velocities(settings.n_particles, std::vector<double>(mkp_instance.n_objects));
    for (auto &particle_vs: velocities) {
        for (auto &v: particle_vs) {
            v = Random::getRealClosed(-settings.max_velocity, settings.max_velocity);
        }
    }

    std::vector<MKPSolution> particles_best = particles;
    MKPSolution global_best = *std::max_element(particles_best.begin(), particles_best.end(),
                                                [](const MKPSolution &a, const MKPSolution &b) {
                                                    return a.total_value < b.total_value;
                                                });
    int iteration_i = 0;
    while (iteration_i < settings.n_iterations) {
        for (int ptc_i = 0; ptc_i < settings.n_particles; ++ptc_i) {
            for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
                int current_obj_state = static_cast<int>(particles[ptc_i].objects[obj_i]);
                int local_obj_state = static_cast<int>(particles_best[ptc_i].objects[obj_i]);
                int global_obj_state = static_cast<int>(global_best.objects[obj_i]);
                double local_change = settings.velocity_local_param * (local_obj_state - current_obj_state);
                double global_change = settings.velocity_global_param * (global_obj_state - current_obj_state);
                velocities[ptc_i][obj_i] += local_change + global_change;
                velocities[ptc_i][obj_i] = Utils::clip(velocities[ptc_i][obj_i], -settings.max_velocity,
                                                       settings.max_velocity);
                if (Random::getRealClosed(0, 1) <= Utils::sigmoid(velocities[ptc_i][obj_i])) {
                    if (!particles[ptc_i].objects[obj_i]) {
                        particles[ptc_i].objects[obj_i] = true;
                        particles[ptc_i].total_value += mkp_instance.object_values[obj_i];
                        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
                            particles[ptc_i].dimension_sizes[dim_i] += mkp_instance.object_sizes[dim_i][obj_i];
                        }
                    }
                } else {
                    if (particles[ptc_i].objects[obj_i]) {
                        particles[ptc_i].objects[obj_i] = false;
                        particles[ptc_i].total_value -= mkp_instance.object_values[obj_i];
                        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
                            particles[ptc_i].dimension_sizes[dim_i] -= mkp_instance.object_sizes[dim_i][obj_i];
                        }
                    }
                }
            }
            assert(check_solution(particles[ptc_i], mkp_instance, true));
            repair_solution(particles[ptc_i], obj_priority_list, mkp_instance);
            assert(check_solution(particles[ptc_i], mkp_instance, false));
            if (particles[ptc_i].total_value > particles_best[ptc_i].total_value) {
                particles_best[ptc_i] = particles[ptc_i];
            }
            if (particles[ptc_i].total_value > global_best.total_value) {
                global_best = particles[ptc_i];
            }
        }
        ++iteration_i;
    }
    return global_best;
}

void Solver::repair_solution(MKPSolution &solution, const std::vector<int> &objects_priority_list,
                             const MKPInstance &mkp_instance) {
    int remove_obj_plist_i = mkp_instance.n_objects - 1;
    while (true) {
        bool apply_drop = false;
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            if (solution.dimension_sizes[dim_i] > mkp_instance.dimension_capacities[dim_i]) {
                apply_drop = true;
                break;
            }
        }
        if (apply_drop) {
            // find first included object according to priority list
            while (!solution.objects[objects_priority_list[remove_obj_plist_i]]) {
                --remove_obj_plist_i;
            }
            solution.objects[objects_priority_list[remove_obj_plist_i]] = false;
            solution.total_value -= mkp_instance.object_values[objects_priority_list[remove_obj_plist_i]];
            for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
                solution.dimension_sizes[dim_i] -= mkp_instance.object_sizes[dim_i][objects_priority_list[remove_obj_plist_i]];
            }
            --remove_obj_plist_i;
        } else { break; }
    }

    for (int add_obj_i: objects_priority_list) {
        // skip already included objects
        if (solution.objects[add_obj_i]) {
            continue;
        }
        bool continue_loop = false;
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            if (solution.dimension_sizes[dim_i] + mkp_instance.object_sizes[dim_i][add_obj_i] >
                mkp_instance.dimension_capacities[dim_i]) {
                // object would violate size constraints
                continue_loop = true;
                break;
            }
        }
        if (continue_loop) { continue; }
        solution.objects[add_obj_i] = true;
        solution.total_value += mkp_instance.object_values[add_obj_i];
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            solution.dimension_sizes[dim_i] += mkp_instance.object_sizes[dim_i][add_obj_i];
        }
    }
}

bool Solver::check_solution(const MKPSolution &solution, const MKPInstance &mkp_instance, bool skip_dim_cap_check) {
    assert(mkp_instance.n_objects == solution.objects.size());

    int true_total_value = 0;
    for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
        assert(0 <= mkp_instance.object_values[obj_i]);
        if (solution.objects[obj_i]) {
            true_total_value += mkp_instance.object_values[obj_i];
        }
    }
    assert(true_total_value == solution.total_value);

    std::vector<int> true_dimension_sizes(mkp_instance.n_dimensions, 0);
    for (int obj_i = 0; obj_i < mkp_instance.n_objects; ++obj_i) {
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            assert(0 <= mkp_instance.object_sizes[dim_i][obj_i]);
            if (solution.objects[obj_i]) {
                true_dimension_sizes[dim_i] += mkp_instance.object_sizes[dim_i][obj_i];
            }
        }
    }
    assert(true_dimension_sizes == solution.dimension_sizes);

    if (!skip_dim_cap_check) {
        for (int dim_i = 0; dim_i < mkp_instance.n_dimensions; ++dim_i) {
            assert(mkp_instance.dimension_capacities[dim_i] >= solution.dimension_sizes[dim_i]);
        }
    }
    return true;
}
