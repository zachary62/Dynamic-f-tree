#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

Matrix::Matrix(std::vector<std::vector<double>> m) {
    _m = m;
}

void Matrix::printSelf(){
    for(vector<double> r: _m){
        for(double i : r){
            cout << fixed << setprecision(2) << i <<" ";
        }
        cout << "\n";
    }
}