#pragma once
#include <vector>

class Matrix
{
public:
    Matrix(std::vector<std::vector<double>> m);
    std::vector<std::vector<double>> _m;
    void printSelf();
};