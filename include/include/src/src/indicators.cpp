#include "indicators.hpp"
#include "utils.hpp"
#include <cmath>
#include <omp.h>
#include <iostream>

std::vector<double> compute_sma(const std::vector<double>& prices, int window) {
    int n = prices.size();
    std::vector<double> sma(n, NAN);
    if (window <= 0 || n < window) return sma;
    auto P = prefix_sum(prices);
    for (int i = window-1; i < n; ++i) {
        double sum = P[i+1] - P[i+1-window];
        sma[i] = sum / window;
    }
    return sma;
}

std::vector<double> compute_rsi_window(const std::vector<double>& prices, int window) {
    int n = prices.size();
    std::vector<double> rsi(n, NAN);
    if (window <= 0 || n <= window) return rsi;
    std::vector<double> gains(n, 0.0), losses(n, 0.0);
    for (int i = 1; i < n; ++i) {
        double d = prices[i] - prices[i-1];
        gains[i] = std::max(0.0, d);
        losses[i] = std::max(0.0, -d);
    }
    auto G = prefix_sum(gains);
    auto L = prefix_sum(losses);
    for (int i = window; i < n; ++i) {
        double avg_gain = (G[i+1] - G[i+1-window]) / window;
        double avg_loss = (L[i+1] - L[i+1-window]) / window;
        if (avg_loss == 0.0) rsi[i] = 100.0;
        else {
            double rs = avg_gain / avg_loss;
            rsi[i] = 100.0 - (100.0 / (1.0 + rs));
        }
    }
    return rsi;
}

std::vector<double> compute_ema(const std::vector<double>& prices, int period) {
    int n = prices.size();
    std::vector<double> ema(n, NAN);
    if (period <= 0 || n == 0) return ema;
    double alpha = 2.0 / (period + 1.0);
    if (n < period) return ema;
    double sum = 0.0;
    for (int i=0;i<period;i++) sum += prices[i];
    ema[period-1] = sum / period;
    for (int i = period; i < n; ++i) {
        ema[i] = alpha * prices[i] + (1 - alpha) * ema[i-1];
    }
    return ema;
}

Indicators process_ticker(const std::vector<double>& close_prices) {
    Indicators out;
    int n = close_prices.size();
    if (n==0) return out;

    out.sma_short = compute_sma(close_prices, 20);
    out.sma_long  = compute_sma(close_prices, 50);
    out.rsi = compute_rsi_window(close_prices, 14);

    auto ema12 = compute_ema(close_prices, 12);
    auto ema26 = compute_ema(close_prices, 26);
    out.macd.assign(n, NAN);
    for (int i=0;i<n;i++) {
        if (!std::isnan(ema12[i]) && !std::isnan(ema26[i])) 
            out.macd[i] = ema12[i] - ema26[i];
    }
    out.macd_signal = compute_ema(out.macd, 9);
    return out;
}
