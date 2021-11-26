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

    BenchmarkData() : problem_id{-1}, known_optimum{0}, n_dimensions{0}, n_objects{0}, times{}, best_found_solutions{} {}
};


class BenchmarkServices {
public:

    // Start times measurement
    static void run();

private:

    static const int TEST_REPETITIONS_NUMBER = 30;

    static std::vector<BenchmarkData> benchmark_algorithm(const std::vector<MKPInstance> &mkp_instances,
                                                          MKPSolution (*mkp_algorithm)(const MKPInstance &mkp_instance,
                                                                                   const SolverSettings &settings),
                                                          const std::string &algorithm_name);

    static double count_time(const std::function<void()> &function);


    static void save_time_data(const std::string &file_name, const std::vector<BenchmarkData> &bdata);

};


#endif //NIOA_PSO_BENCHMARKSERVICES_H
