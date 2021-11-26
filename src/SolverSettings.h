#ifndef NIOA_PSO_SOLVERSETTINGS_H
#define NIOA_PSO_SOLVERSETTINGS_H

struct SolverSettings {
    bool set_time_limit = true;
    int time_limit = 30000; // ms

    bool work_until_converged = false;
};

#endif //NIOA_PSO_SOLVERSETTINGS_H
