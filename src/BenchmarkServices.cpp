#include "BenchmarkServices.h"
#include "DataLoader.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>

#include "Solver.h"
#include "Utils.h"

using std::cout;
using std::endl;

void BenchmarkServices::run() {
    auto mkp_instances = DataLoader::load_mknap2("../data/mknap2.txt", {0, 1, 2, 3, 4, 5, 6, 7, 8, 19, 47});

    auto simulated_annealing_time_data = benchmark_algorithm(mkp_instances, Solver::simulated_annealing,
                                                             "simulated_annealing");
    save_time_data("simulated_annealing", "Simulated Annealing", simulated_annealing_time_data);

    auto particle_swarm_time_data = benchmark_algorithm(mkp_instances, Solver::particle_swarm, "particle_swarm");
    save_time_data("particle_swarm", "Particle Swarm Optimization", particle_swarm_time_data);
}

std::vector<BenchmarkData> BenchmarkServices::benchmark_algorithm(const std::vector<MKPInstance> &mkp_instances,
                                                                  MKPSolution (*mkp_algorithm)(
                                                                          const MKPInstance &mkp_instance,
                                                                          const SolverSettings &settings),
                                                                  const std::string &algorithm_name) {
    cout << std::string(10, '-') << "Benchmark of \"" + algorithm_name + "\"" + " started"
         << std::string(10, '-') << endl;

    MKPSolution found_solution;
    std::vector<BenchmarkData> bdata;
    BenchmarkData instance_bdata;
    for (const auto &mkp_instance: mkp_instances) {
        std::cout << "Processing instance " << mkp_instance.id;
        instance_bdata.problem_id = mkp_instance.id;
        instance_bdata.known_optimum = mkp_instance.known_optimum;
        instance_bdata.n_dimensions = mkp_instance.n_dimensions;
        instance_bdata.n_objects = mkp_instance.n_objects;
        for (int repeats = 0; repeats < TEST_REPETITIONS_NUMBER; ++repeats) {
            double time = count_time([&]() -> void {
                found_solution = mkp_algorithm(mkp_instance, SolverSettings());
            });
            instance_bdata.best_found_solutions.push_back(found_solution.total_value);
            instance_bdata.times.push_back(time);
            cout << '.';
        }
        auto default_precision = cout.precision();
        cout.precision(2);
        cout << endl;
        cout << "Elapsed time [ms]: " << Utils::mean(instance_bdata.times) << " +- " << Utils::stddev(instance_bdata.times)
             << endl;
        std::vector<long double> relative_errors;
        for (const auto &el : instance_bdata.best_found_solutions) {
            relative_errors.push_back(Utils::relative_error(instance_bdata.known_optimum, el));
        }
        cout.precision(6);
        cout << "Relative error [%]: " << Utils::mean(relative_errors) << " +- " << Utils::stddev(relative_errors)
             << endl;
        cout << endl;
        cout.precision(default_precision);

        bdata.push_back(instance_bdata);
        instance_bdata = BenchmarkData();
    }
    cout << std::string(10, '-') << "Benchmark of \"" + algorithm_name + "\"" + " finished"
         << std::string(10, '-') << endl;
    return bdata;
}

double BenchmarkServices::count_time(const std::function<void()> &function) {
    std::chrono::high_resolution_clock::time_point start, finish;
    std::chrono::duration<double, std::milli> elapsed;
    start = std::chrono::high_resolution_clock::now();
    function();
    finish = std::chrono::high_resolution_clock::now();
    elapsed = finish - start;
    return elapsed.count();
}

void BenchmarkServices::save_time_data(const std::string &file_name, const std::string &algorithm_name,
                                       const std::vector<BenchmarkData> &bdata) {
    const char sep = ',';
    const std::string benchmark_file_name = file_name + "_benchmark.csv";

    std::ofstream benchmark_file("../benchmark_results/" + benchmark_file_name);
    if (!benchmark_file.is_open()) {
        throw std::invalid_argument(benchmark_file_name + " was not opened!");
    }

    benchmark_file << "Instance ID" << sep;
    benchmark_file << "Dimensions" << sep;
    benchmark_file << "Objects" << sep;
    benchmark_file << "Best algorithm solution" << sep;
    benchmark_file << "Known optimum" << sep;
    benchmark_file << "Time" << sep;
    benchmark_file << "Time unit" << endl;

    for (const auto &instance_bdata: bdata) {
        for (unsigned i = 0; i < instance_bdata.best_found_solutions.size(); ++i) {
            benchmark_file << instance_bdata.problem_id << sep;
            benchmark_file << instance_bdata.n_dimensions << sep;
            benchmark_file << instance_bdata.n_objects << sep;
            benchmark_file << instance_bdata.best_found_solutions[i] << sep;
            benchmark_file << instance_bdata.known_optimum << sep;
            benchmark_file << instance_bdata.times[i] << sep;
            benchmark_file << "ms" << endl;
        }
    }

    benchmark_file.close();
    cout << benchmark_file_name << " written successfully!" << endl;
}