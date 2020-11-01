#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <vector>

using namespace std;

Matrix::Matrix(std::vector<std::vector<double>> m) {
    _m = m;
}

Matrix* Matrix::rightMultiply(Matrix* right){
    vector<vector<double>> right_vec = right->_m;
    vector<vector<double>> result;
    unsigned int a = _m.size();
    unsigned int b = _m[0].size();
    if(b != right_vec.size()){
        cout<< "Invalid multiplication!\n";
		exit(1);
    }
    unsigned int c = right_vec[0].size();
    for(unsigned int i = 0; i < a; i++){
        vector<double> result_row;
        for(unsigned int j = 0; j < c; j++){
            double sum = 0;
            for(unsigned int k = 0; k < b; k++){
                sum += _m[i][k] * right_vec[k][j];
            }
            result_row.push_back(sum);
        }
        result.push_back(result_row);
    }

    Matrix* mx = new Matrix(result);
    return mx;
}

Matrix* Matrix::cofactor(){
    vector<vector<double>> result;
    unsigned int a = _m.size();
    unsigned int b = _m[0].size();

    for(unsigned int i = 0; i < b; i++){
        vector<double> result_row(b,0);
        result.push_back(result_row);
    }

    for(unsigned int i = 0; i < b; i++){
        for(unsigned int j = i; j < b; j++){
            double cell_result = 0;
            for(unsigned int k = 0; k < a; k++){
                cell_result += _m[k][i] * _m[k][j]; 
            }


            // just calculate half the matrix
            result[i][j] = cell_result;
            result[j][i] = cell_result;
        }
    }
   
    Matrix* mx = new Matrix(result);
    return mx;
}



void Matrix::printSelf(){
    for(vector<double> r: _m){
        for(double i : r){
            cout << fixed << setprecision(2) << i <<" ";
        }
        cout << "\n";
    }
}