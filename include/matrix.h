#pragma once
#include <vector>

class Matrix
{
public:
    Matrix(double* m, int num_row, int num_column);
    Matrix* rightMultiply(Matrix* right);
    Matrix* cofactor();
    double* _m;
    int _num_row;
    int _num_column;
    void printSelf();
};