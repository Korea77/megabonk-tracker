#include <random>
#include <limits>
#include <cstdint>

int GenerateRandomInt() {
    std::random_device rd;                   // �r�d�o entropii
    std::mt19937 gen(rd());                  // generator Mersenne Twister 32-bit
    std::uniform_int_distribution<int> dist(INT32_MIN, INT32_MAX); // pe�ny zakres int
    return dist(gen);
}