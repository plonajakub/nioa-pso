#ifndef NIOA_PSO_SOLVERSETTINGS_H
#define NIOA_PSO_SOLVERSETTINGS_H

struct SolverSettings {
    int n_iterations = 1000;
    int n_particles = 10;
    double max_velocity = 100.0;
    double velocity_local_param = 4.0;
    double velocity_global_param = 1.0;
};

#endif //NIOA_PSO_SOLVERSETTINGS_H
