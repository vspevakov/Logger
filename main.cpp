#include <iostream>
#include "logger.hpp"

int main() {
    Logger::Instance()->SetLevel(NORMAL);
    std::cout << "Hello, World!" << std::endl;
    LogError << "bibf" << 228 << std::endl;
    LogNormal << "norm" << 228 << std::endl;
    LogWarn << "bol'no" << 228 << std::endl;
    return 0;
}
