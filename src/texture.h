#pragma once
#include <vector>
#include <cstdint>

uint32_t LoadPNG_1D(const char* path);

void LoadTexVectorVar(std::vector<uint32_t>& v, int n, ...);
