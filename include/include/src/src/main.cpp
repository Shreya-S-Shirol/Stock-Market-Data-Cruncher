#include "indicators.hpp"
#include "utils.hpp"
#include <omp.h>
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    int num_tickers = 100;
    int points_per_ticker = 10000;
    auto data = load_sample_data(num_tickers, points_per_ticker);
    std::vector<Indicators> results(num_tickers);

    double t0 = omp_get_wtime();
    #pragma omp parallel for schedule(dynamic)
    for (int t = 0; t < num_tickers; ++t) {
        results[t] = process_ticker(data[t]);
    }
    double t1 = omp_get_wtime();
    std::cout << "Processed " << num_tickers << " tickers in "
              << (t1 - t0) << " seconds using "
              << omp_get_max_threads() << " threads.\n";

    // Example: SMA crossover alerts
    for (int t = 0; t < num_tickers; t++) {
        auto &s = results[t];
        int n = s.sma_short.size();
        for (int i = 1; i < n; ++i) {
            if (std::isnan(s.sma_short[i]) || std::isnan(s.sma_long[i]) ||
                std::isnan(s.sma_short[i-1]) || std::isnan(s.sma_long[i-1]))
                continue;

            bool prev_below = s.sma_short[i-1] <= s.sma_long[i-1];
            bool now_above  = s.sma_short[i] > s.sma_long[i];
            if (prev_below && now_above) {
                std::cout << "Ticker " << t << ": SMA20 crossed above SMA50 at index " << i << "\n";
            }
        }
    }

    return 0;
}
