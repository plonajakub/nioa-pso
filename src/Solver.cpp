#include "Solver.h"
#include <vector>

MKPSolution Solver::greedy(const MKPInstance &mkp_instance, const SolverSettings &settings) {
    return {std::vector<bool>(mkp_instance.n_objects, false), -1};
}

MKPSolution Solver::particle_swarm(const MKPInstance &mkp_instance, const SolverSettings &settings) {
    return {std::vector<bool>(mkp_instance.n_objects, false), -1};
}
