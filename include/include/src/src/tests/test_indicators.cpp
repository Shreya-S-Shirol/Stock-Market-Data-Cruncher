#include "indicators.hpp"
#include <cassert>
#include <iostream>

int main() {
    std::vector<double> prices = {1,2,3,4,5,6,7,8,9,10};
    auto sma = compute_sma(prices, 3);
    assert(std::abs(sma[4] - 4.0) < 1e-9); // sanity check
    std::cout << "SMA test passed ✅\n";
    return 0;
}
