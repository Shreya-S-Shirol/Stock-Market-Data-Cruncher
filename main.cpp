#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <cmath>
#include <chrono>
#include <omp.h>

using namespace std;

// -------------------------
// Structure to store stock data
// -------------------------
struct StockData {
    string date;
    double close;
};

// -------------------------
// Function Declarations
// -------------------------
bool loadStockData(const string &filename, vector<StockData> &data);
vector<double> computeSMA(const vector<double> &prices, int window);
vector<double> computeRSI(const vector<double> &prices, int period);
void computeMACD(const vector<double> &prices, vector<double> &macd, vector<double> &signal);
void generateAlerts(const vector<double> &smaShort, const vector<double> &smaLong, 
                    const vector<double> &macd, const vector<double> &signal);
void logAlert(const string &message);
void benchmark(const string &label, double start, double end);

// -------------------------
// Utility: Alert logging
// -------------------------
void logAlert(const string &message) {
    ofstream logFile("alerts.txt", ios::app);
    if (logFile.is_open()) {
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        logFile << "[" << ctime(&now) << "] " << message << endl;
        logFile.close();
    } else {
        cerr << "[ERROR] Could not open alerts.txt for logging.\n";
    }
}

// -------------------------
// Utility: Performance benchmarking
// -------------------------
void benchmark(const string &label, double start, double end) {
    double timeTaken = end - start;
    cout << label << " completed in " << timeTaken << " seconds.\n";
}

// -------------------------
// Load CSV data
// -------------------------
bool loadStockData(const string &filename, vector<StockData> &data) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[ERROR] Could not open file: " << filename << endl;
        return false;
    }

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        stringstream ss(line);
        string date, closeStr;
        getline(ss, date, ',');
        getline(ss, closeStr, ',');

        try {
            StockData entry;
            entry.date = date;
            entry.close = stod(closeStr);
            data.push_back(entry);
        } catch (...) {
            cerr << "[WARNING] Skipping invalid line: " << line << endl;
        }
    }

    file.close();
    if (data.empty()) {
        cerr << "[ERROR] No valid data found in file.\n";
        return false;
    }

    cout << "[INFO] Loaded " << data.size() << " records successfully.\n";
    return true;
}

// -------------------------
// Compute Simple Moving Average (Parallel)
// -------------------------
vector<double> computeSMA(const vector<double> &prices, int window) {
    int n = prices.size();
    vector<double> sma(n, 0.0);

    #pragma omp parallel for
    for (int i = window - 1; i < n; i++) {
        double sum = 0.0;
        for (int j = i - window + 1; j <= i; j++)
            sum += prices[j];
        sma[i] = sum / window;
    }

    return sma;
}

// -------------------------
// Compute RSI (Sequential for simplicity)
// -------------------------
vector<double> computeRSI(const vector<double> &prices, int period) {
    int n = prices.size();
    vector<double> rsi(n, 0.0);

    for (int i = period; i < n; i++) {
        double gain = 0.0, loss = 0.0;
        for (int j = i - period + 1; j <= i; j++) {
            double diff = prices[j] - prices[j - 1];
            if (diff > 0)
                gain += diff;
            else
                loss -= diff;
        }

        double avgGain = gain / period;
        double avgLoss = loss / period;
        double rs = (avgLoss == 0) ? 0 : avgGain / avgLoss;
        rsi[i] = 100 - (100 / (1 + rs));
    }

    return rsi;
}

// -------------------------
// Compute MACD and Signal Line (Parallel)
// -------------------------
void computeMACD(const vector<double> &prices, vector<double> &macd, vector<double> &signal) {
    int n = prices.size();
    vector<double> ema12(n, 0.0), ema26(n, 0.0);
    double alpha12 = 2.0 / (12 + 1);
    double alpha26 = 2.0 / (26 + 1);

    // Exponential Moving Averages
    ema12[0] = prices[0];
    ema26[0] = prices[0];

    #pragma omp parallel for
    for (int i = 1; i < n; i++) {
        ema12[i] = (prices[i] - ema12[i - 1]) * alpha12 + ema12[i - 1];
        ema26[i] = (prices[i] - ema26[i - 1]) * alpha26 + ema26[i - 1];
    }

    // MACD = EMA12 - EMA26
    macd.resize(n);
    #pragma omp parallel for
    for (int i = 0; i < n; i++)
        macd[i] = ema12[i] - ema26[i];

    // Signal Line = 9-day EMA of MACD
    double alpha9 = 2.0 / (9 + 1);
    signal.resize(n);
    signal[0] = macd[0];

    for (int i = 1; i < n; i++)
        signal[i] = (macd[i] - signal[i - 1]) * alpha9 + signal[i - 1];
}

// -------------------------
// Generate Alerts
// -------------------------
void generateAlerts(const vector<double> &smaShort, const vector<double> &smaLong, 
                    const vector<double> &macd, const vector<double> &signal) {
    cout << "\n[INFO] Generating alerts...\n";
    for (size_t i = 1; i < smaShort.size(); i++) {
        // SMA Crossover Alert
        if (smaShort[i - 1] < smaLong[i - 1] && smaShort[i] > smaLong[i]) {
            string msg = "SMA Crossover BUY signal detected.";
            cout << msg << endl;
            logAlert(msg);
        } else if (smaShort[i - 1] > smaLong[i - 1] && smaShort[i] < smaLong[i]) {
            string msg = "SMA Crossover SELL signal detected.";
            cout << msg << endl;
            logAlert(msg);
        }

        // MACD Signal Alert
        if (macd[i - 1] < signal[i - 1] && macd[i] > signal[i]) {
            string msg = "MACD BUY signal detected.";
            cout << msg << endl;
            logAlert(msg);
        } else if (macd[i - 1] > signal[i - 1] && macd[i] < signal[i]) {
            string msg = "MACD SELL signal detected.";
            cout << msg << endl;
            logAlert(msg);
        }
    }
}

// -------------------------
// Main Function
// -------------------------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "[USAGE] ./cruncher <dataset.csv>\n";
        return 1;
    }

    string filename = argv[1];
    vector<StockData> data;

    // Load Data
    if (!loadStockData(filename, data)) return 1;

    vector<double> prices;
    for (auto &d : data) prices.push_back(d.close);

    // Sequential Benchmark
    double startSeq = omp_get_wtime();
    auto smaSeq = computeSMA(prices, 20);
    auto rsiSeq = computeRSI(prices, 14);
    vector<double> macdSeq, signalSeq;
    computeMACD(prices, macdSeq, signalSeq);
    double endSeq = omp_get_wtime();
    benchmark("[SEQUENTIAL] Indicator computation", startSeq, endSeq);

    // Parallel Benchmark
    double startPar = omp_get_wtime();
    auto smaShort = computeSMA(prices, 10);
    auto smaLong = computeSMA(prices, 30);
    vector<double> macdPar, signalPar;
    computeMACD(prices, macdPar, signalPar);
    auto rsiPar = computeRSI(prices, 14);
    double endPar = omp_get_wtime();
    benchmark("[PARALLEL] Indicator computation", startPar, endPar);

    cout << "\n[INFO] Speedup: " << (endSeq - startSeq) / (endPar - startPar) << "x\n";

    // Generate Alerts
    generateAlerts(smaShort, smaLong, macdPar, signalPar);

    cout << "\n[INFO] Alerts logged in alerts.txt\n";
    cout << "[INFO] Program finished successfully.\n";
    return 0;
}
