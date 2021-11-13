#ifndef NIOA_PSO_UTILS_H
#define NIOA_PSO_UTILS_H

#include <vector>


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

    static long double absolute_error(long double reference_value, long double current_value) {
        return current_value - reference_value;
    }

    // In %
    static long double relative_error(long double reference_value, long double current_value) {
        return Utils::absolute_error(reference_value, current_value) / reference_value * 100;
    }
};


#endif //NIOA_PSO_UTILS_H
