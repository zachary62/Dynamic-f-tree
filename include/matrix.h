#pragma once
#include <vector>

class Matrix
{
public:
    Matrix();
    Matrix(double* m, int num_row, int num_column);
    Matrix* rightMultiply(Matrix* right);
    Matrix* cofactor();

    Matrix* shallowCopy();
    Matrix* deepCopy();
    void deepCopy(Matrix* target);

    // add in place
    void add(Matrix* right);
    void minus(Matrix* right);
    void minusedby(Matrix* left);
    void minusScalar(Matrix* right, int start, int end);
    void update(double* m, int num_row, int num_column);
    void divide(double v);
    void TransposeOne();
    void inverse();

    //
    double trace();
    double toDouble();
    

    double* _m;
    int _num_row;
    int _num_column;
    void printSelf();
    void printShape();
};