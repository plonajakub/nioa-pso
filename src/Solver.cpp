#include "Solver.h"
#include <vector>

#include <thread>
#include <chrono>
using namespace std::chrono_literals;

MKPSolution Solver::greedy(const MKPInstance &mkp_instance, const SolverSettings &settings) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20 + static_cast<int>(static_cast<double>(rand()) / RAND_MAX * 20)));
    return {std::vector<bool>(mkp_instance.n_objects, false), 1000 + static_cast<int>(static_cast<double>(rand()) / RAND_MAX * 1000)};
}

MKPSolution Solver::particle_swarm(const MKPInstance &mkp_instance, const SolverSettings &settings) {
    std::this_thread::sleep_for(std::chrono::milliseconds(50 + static_cast<int>(static_cast<double>(rand()) / RAND_MAX * 50)));
    return {std::vector<bool>(mkp_instance.n_objects, false), 10000 + static_cast<int>(static_cast<double>(rand()) / RAND_MAX * 1000)};
}
