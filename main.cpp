#include <iostream>
#include "logger.hpp"

int main() {
    Logger::Instance()->SetLevel(NORMAL);
    std::cout << "Hello, World!" << std::endl;
    LogError << "error" << std::endl;
    LogNormal << "normal" << std::endl;
    LogWarn << "warn" << std::endl;
    return 0;
}
