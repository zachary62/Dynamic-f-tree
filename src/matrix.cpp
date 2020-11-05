#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

Matrix::Matrix(double* m, int num_row, int num_column) {
    _m = m;
    _num_row = num_row;
    _num_column = num_column;
}

Matrix* Matrix::rightMultiply(Matrix* right){
    double* right_vec = right->_m;
    int a = _num_row;
    int b = _num_column;

    if(b != right-> _num_row){
        cout<< "Invalid multiplication!\n";
		exit(1);
    }
    int c = right->  _num_column;

    double* result = new double[a*c];

    for(int i = 0; i < a; i++){
        for(int j = 0; j < c; j++){
            for(int k = 0; k < b; k++){
                result[i*c + j] += _m[i*b +k] * right_vec[k*c + j];
            }
            
        }
    }

    // memory!!
    Matrix* mx = new Matrix(result,a,c);
    return mx;
}

Matrix* Matrix::cofactor(){
    
    int a = _num_row;
    int b = _num_column;

    double* result = new double[b*b];

    for(int i = 0; i < b; i++){
        for(int j = i; j < b; j++){
            double cell_result = 0;
            for(int k = 0; k < a; k++){
                cell_result += _m[k*b + i] * _m[k*b + j]; 
            }


            // just calculate half the matrix
            result[i*b + j] = cell_result;
            result[j*b + i] = cell_result;
        }
    }
   
    Matrix* mx = new Matrix(result, b, b);
    return mx;
}



void Matrix::printSelf(){
    for(int i = 0; i < _num_row ; i++){
        for(int j = 0; j <  _num_column; j++){
            cout << fixed << setprecision(2) << _m[i* _num_column + j] <<" ";
        }
        cout << "\n";
    }
}