#pragma once
#include <vector>

class Matrix
{
public:
    Matrix(std::vector<std::vector<double>> m);
    Matrix* rightMultiply(Matrix* right);
    Matrix* cofactor();
    std::vector<std::vector<double>> _m;
    void printSelf();
};