#include <set>
#include <random>
#include <iostream>
#include "hll/hyper_log_log.hxx"

double relative_error(int expected, int got) {
    return abs(got - expected) / (double) expected;
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());

    constexpr int N = (int) 1e6;

    double error_count = 0.0;
    int count = 0;


    hll::hyper_log_log<int, 12> counter{};
    for (int k : {100, 1000, 10000, N / 10, N, N * 10, N * 100, N * 1000}) {
        std::uniform_int_distribution<> dis(1, k);
        std::set<int> all;

        for (int i = 0; i < N; i++) {
            int value = dis(gen);
            all.insert(value);
            counter.add(value);
        }
        int expected = (int) all.size();
        int counter_result = counter.count();
        double error = relative_error(expected, counter_result);
        error_count += error;
        count += 1;
        printf("%d numbers in range [1 .. %d], %d uniq, %d result, %.5f relative error\n", N, k, expected, counter_result, error);

        counter.clear();
    }

    printf("Average error: %.5f\n", error_count / count);
    printf("Paper estimated error: %.5f\n", counter.get_relative_error());
    return 0;
}