# Stock-Market-Data-Cruncher
The stock market generates a mountain of data every day, and analyzing it on a normal computer can be painfully slow. Our tool, Stock Market Data Cruncher, fixes this by harnessing the power of parallel computing with OpenMP.
# 🧮 Stock Market Data Cruncher (OpenMP)

A high-performance C++ tool for **parallel stock data analysis** using OpenMP.

It computes common trading indicators — **SMA**, **RSI**, and **MACD** — across massive datasets in parallel, dramatically speeding up computation.

---

## 🚀 Features

- Parallelized across tickers using OpenMP
- Computes:
  - Simple Moving Average (SMA)
  - Relative Strength Index (RSI)
  - Moving Average Convergence Divergence (MACD)
- Detects SMA crossovers as trading alerts
- Easy to extend with more indicators
- Includes performance benchmarking

---

## 🧰 Build Instructions

```bash
git clone https://github.com/<your-username>/StockMarketDataCruncher.git
cd StockMarketDataCruncher
mkdir build && cd build
cmake ..
make
./stock_cruncher
