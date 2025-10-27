#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

std::vector<double> prefix_sum(const std::vector<double>& a);
std::vector<std::vector<double>> load_sample_data(int num_tickers, int points_per_ticker);

#endif
