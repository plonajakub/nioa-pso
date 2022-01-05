#ifndef NIOA_PSO_SOLVERSETTINGS_H
#define NIOA_PSO_SOLVERSETTINGS_H

struct SolverSettings {
    int n_iterations = 2000;
    int n_particles = 150;
    double max_velocity = 10.0;
    double velocity_local_param = 2.0;
    double velocity_global_param = 0.1;
};

#endif //NIOA_PSO_SOLVERSETTINGS_H
