#ifndef NIOA_PSO_BENCHMARKSERVICES_H
#define NIOA_PSO_BENCHMARKSERVICES_H

#include <functional>
#include <vector>
#include <string>

#include "mkp.h"
#include "SolverSettings.h"

struct BenchmarkData {
    int problem_id;
    int known_optimum;
    int n_dimensions;
    int n_objects;
    std::vector<double> times;
    std::vector<int> best_found_solutions; // target function value

    BenchmarkData() : problem_id{-1}, known_optimum{0}, n_dimensions{0}, n_objects{0}, times{},
                      best_found_solutions{} {}
};


class BenchmarkServices {
public:

    // Start times measurement
    static void run();

private:

    static const int TEST_REPETITIONS_NUMBER = 40;

    static std::vector<BenchmarkData> benchmark_algorithm(const std::string &algorithm_name,
                                                          MKPSolution (*mkp_algorithm)(
                                                                  const MKPInstance &,
                                                                  const SolverSettings &),
                                                          const std::vector<MKPInstance> &mkp_instances,
                                                          const SolverSettings &s_settings);

    static double count_time(const std::function<void()> &function);


    static void save_time_data(const std::string &file_name, const std::vector<BenchmarkData> &bdata);

};


#endif //NIOA_PSO_BENCHMARKSERVICES_H
