#ifndef NIOA_PSO_SOLVER_H
#define NIOA_PSO_SOLVER_H

#include "mkp.h"
#include "SolverSettings.h"

class Solver {
public:
    static MKPSolution greedy(const MKPInstance &mkp_instance, const SolverSettings &settings);
    static MKPSolution particle_swarm(const MKPInstance &mkp_instance, const SolverSettings &settings);
private:
    static void repair_solution(MKPSolution &solution, const std::vector<int> &objects_priority_list,
                                const MKPInstance &mkp_instance);
};


#endif //NIOA_PSO_SOLVER_H
