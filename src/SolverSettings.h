#ifndef NIOA_PSO_SOLVERSETTINGS_H
#define NIOA_PSO_SOLVERSETTINGS_H

struct SolverSettings {
//    bool set_time_limit = true;
//    int time_limit = 30000; // ms
//
//    bool work_until_converged = false;
    int n_particles = 10;
    double max_velocity = 100.0;
    int n_iterations = 1000;

    double velocity_local_param = 4.0;
    double velocity_global_param = 1.0;
};

#endif //NIOA_PSO_SOLVERSETTINGS_H
