#ifndef NIOA_PSO_SOLVER_H
#define NIOA_PSO_SOLVER_H

#include "mkp.h"
#include "SolverSettings.h"

class Solver {
public:
    static MKPSolution simulated_annealing(const MKPInstance &mkp_instance, const SolverSettings &settings);
    static MKPSolution particle_swarm(const MKPInstance &mkp_instance, const SolverSettings &settings);
};


#endif //NIOA_PSO_SOLVER_H
