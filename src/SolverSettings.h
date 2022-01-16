#ifndef NIOA_PSO_SOLVERSETTINGS_H
#define NIOA_PSO_SOLVERSETTINGS_H

struct SolverSettings {
    int n_iterations = 2000;
    int n_particles = 200;
    double max_velocity = 3.0;
    double velocity_local_param = 0.25;
    double velocity_global_param = 0.01;
};

#endif //NIOA_PSO_SOLVERSETTINGS_H
