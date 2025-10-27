#ifndef INDICATORS_HPP
#define INDICATORS_HPP

#include <vector>
#include <cmath>

struct Indicators {
    std::vector<double> sma_short, sma_long, rsi, macd, macd_signal;
};

std::vector<double> compute_sma(const std::vector<double>& prices, int window);
std::vector<double> compute_rsi_window(const std::vector<double>& prices, int window);
std::vector<double> compute_ema(const std::vector<double>& prices, int period);
Indicators process_ticker(const std::vector<double>& close_prices);

#endif
