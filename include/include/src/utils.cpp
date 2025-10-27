#include "utils.hpp"
#include <vector>
#include <cstdlib>
#include <cmath>

std::vector<double> prefix_sum(const std::vector<double>& a) {
    int n = (int)a.size();
    std::vector<double> p(n+1);
    p[0] = 0.0;
    for (int i=0; i<n; i++) p[i+1] = p[i] + a[i];
    return p;
}

std::vector<std::vector<double>> load_sample_data(int num_tickers, int points_per_ticker) {
    std::vector<std::vector<double>> data(num_tickers);
    for (int t=0; t<num_tickers; t++) {
        data[t].resize(points_per_ticker);
        double v = 100.0 + t;
        for (int i=0; i<points_per_ticker; i++) {
            v += (sin(i*0.01) + (rand()%100 - 50) * 0.001);
            data[t][i] = v;
        }
    }
    return data;
}
