#include "BenchmarkServices.h"

#include "DataLoader.h"
#include "Solver.h"
#include "Utils.h"

#include <iostream>
#include <fstream>
#include <chrono>

using std::cout;
using std::endl;


void BenchmarkServices::run() {
    auto mkp_instances = DataLoader::load_mknap2("../data/mknap2.txt", {});

    auto greedy_time_data = benchmark_algorithm("greedy__default", Solver::greedy, mkp_instances, SolverSettings());
    save_time_data("greedy__default", greedy_time_data);

    auto particle_swarm_time_data = benchmark_algorithm("particle_swarm__default", Solver::particle_swarm,
                                                        mkp_instances, SolverSettings());
    save_time_data("particle_swarm__default", particle_swarm_time_data);

    int test_iteration_values[] = {100, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000};
    for (auto tiv: test_iteration_values) {
        SolverSettings settings;
        settings.n_iterations = tiv;
        auto pstd = benchmark_algorithm("particle_swarm__iter_" + std::to_string(tiv), Solver::particle_swarm,
                                        mkp_instances, settings);
        save_time_data("particle_swarm__iter_" + std::to_string(tiv), pstd);
    }

    int test_particle_values[] = {5, 25, 50, 75, 100, 125, 150, 175, 200, 250, 300};
    for (auto tpv: test_particle_values) {
        SolverSettings settings;
        settings.n_particles = tpv;
        auto pstd = benchmark_algorithm("particle_swarm__ptc_" + std::to_string(tpv), Solver::particle_swarm,
                                        mkp_instances, settings);
        save_time_data("particle_swarm__ptc_" + std::to_string(tpv), pstd);
    }

    double test_velocity_values[] = {0.1, 0.25, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 5.0, 7.0, 10.0, 15.0, 20.0};
    for (auto tvv: test_velocity_values) {
        SolverSettings settings;
        settings.max_velocity = tvv;
        auto tvv_str = std::to_string(tvv);
        tvv_str.replace(tvv_str.find('.'), 1, "-");
        auto pstd = benchmark_algorithm("particle_swarm__mv_" + tvv_str, Solver::particle_swarm,
                                        mkp_instances, settings);
        save_time_data("particle_swarm__mv_" + tvv_str, pstd);
    }

    double test_velocity_local_values[] = {0.01, 0.05, 0.15, 0.25, 0.5, 1, 1.5, 2};
    double test_velocity_global_values[] = {0.01, 0.05, 0.15, 0.25, 0.5, 1, 1.5, 2};
    for (auto tvlv: test_velocity_local_values) {
        for (auto tvgv: test_velocity_global_values) {
            SolverSettings settings;
            settings.velocity_local_param = tvlv;
            auto tvlv_str = std::to_string(tvlv);
            tvlv_str.replace(tvlv_str.find('.'), 1, "-");
            settings.velocity_global_param = tvgv;
            auto tvgv_str = std::to_string(tvgv);
            tvgv_str.replace(tvgv_str.find('.'), 1, "-");
            auto pstd = benchmark_algorithm("particle_swarm__vl_" + tvlv_str + "__vg_" + tvgv_str,
                                            Solver::particle_swarm, mkp_instances, settings);
            save_time_data("particle_swarm__vl_" + tvlv_str + "__vg_" + tvgv_str, pstd);
        }
    }
}

std::vector<BenchmarkData> BenchmarkServices::benchmark_algorithm(const std::string &algorithm_name,
                                                                  MKPSolution (*mkp_algorithm)(
                                                                          const MKPInstance &,
                                                                          const SolverSettings &),
                                                                  const std::vector<MKPInstance> &mkp_instances,
                                                                  const SolverSettings &s_settings) {
    cout << std::string(10, '-') << "Benchmark of \"" + algorithm_name + "\"" + " started"
         << std::string(10, '-') << endl;

    std::vector<BenchmarkData> bdata;
    for (const auto &mkp_instance: mkp_instances) {
        std::cout << "Processing instance " << mkp_instance.id;
        BenchmarkData instance_bdata;
        instance_bdata.problem_id = mkp_instance.id;
        instance_bdata.known_optimum = mkp_instance.known_optimum;
        instance_bdata.n_dimensions = mkp_instance.n_dimensions;
        instance_bdata.n_objects = mkp_instance.n_objects;
        for (int repeats = 0; repeats < TEST_REPETITIONS_NUMBER; ++repeats) {
            MKPSolution found_solution;
            double time = count_time([&]() -> void {
                found_solution = mkp_algorithm(mkp_instance, s_settings);
            });
//            cout << "Solution: ";
//            for (auto el : found_solution.objects) {
//                cout << el << " ";
//            }
//            cout << endl << "Total value: " << found_solution.total_value << endl;
            instance_bdata.best_found_solutions.push_back(found_solution.total_value);
            instance_bdata.times.push_back(time);
            cout << '.';
        }
        bdata.push_back(instance_bdata);

        auto default_precision = cout.precision();
        cout.precision(2);
        cout << endl;
        cout << "Elapsed time [ms]: " << Utils::mean(instance_bdata.times) << " +- "
             << Utils::stddev(instance_bdata.times) << endl;

        std::vector<long double> relative_errors;
        for (const auto &el: instance_bdata.best_found_solutions) {
            relative_errors.push_back(Utils::relative_error(instance_bdata.known_optimum, el));
        }
        cout.precision(6);
        cout << "Relative error [%]: " << Utils::mean(relative_errors) << " +- " << Utils::stddev(relative_errors)
             << endl;
        cout << endl;
        cout.precision(default_precision);
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

void BenchmarkServices::save_time_data(const std::string &file_name, const std::vector<BenchmarkData> &bdata) {
    const char sep = ',';
    const std::string benchmark_file_name = file_name + "__benchmark.csv";

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