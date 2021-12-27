#ifndef NIOA_PSO_UTILS_H
#define NIOA_PSO_UTILS_H

#include <vector>
#include <cmath>


class Utils {
public:
    template<class T>
    static long double sum(const std::vector<T> &items) {
        long double sum = 0;
        for (auto el: items) {
            sum += el;
        }
        return sum;
    }

    template<class T>
    static long double mean(const std::vector<T> &items) {
        return sum(items) / static_cast<long double>(items.size());
    }

    template<class T>
    static long double stddev(const std::vector<T> &items) {
        long double var = 0;
        long double _mean = Utils::mean(items);
        for (long double item: items) {
            var += (item - _mean) * (item - _mean);
        }
        var /= static_cast<long double>(items.size());
        return sqrt(var);
    }

    // positive = over reference
    // negative = under reference
    static long double absolute_error(long double reference_value, long double current_value) {
        return current_value - reference_value;
    }

    // positive = over reference
    // negative = under reference
    // In %
    static long double relative_error(long double reference_value, long double current_value) {
        return Utils::absolute_error(reference_value, current_value) / reference_value * 100;
    }

    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    static double clip(double x, double left, double right) {
        if (x > right) { return right; }
        else if (x < left) { return left; }
        else { return x; }
    }
};


#endif //NIOA_PSO_UTILS_H
