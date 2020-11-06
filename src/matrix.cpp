#include "matrix.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <Accelerate/Accelerate.h>

using namespace std;

Matrix::Matrix(double* m, int num_row, int num_column) {
    _m = m;
    _num_row = num_row;
    _num_column = num_column;
}

Matrix* Matrix::rightMultiply(Matrix* right){

    CBLAS_ORDER layout = CblasRowMajor;
    CBLAS_TRANSPOSE tp = CblasNoTrans;

    double* right_vec = right->_m;
    int a = _num_row;
    int b = _num_column;

    if(b != right-> _num_row){
        cout<< "Invalid multiplication!\n";
		exit(1);
    }
    int c = right->  _num_column;

    double* result = new double[a*c];

    cblas_dgemm(layout,tp, tp, a, c, b, 1, _m, b, right_vec, c, 0, result, c);


    // for(int i = 0; i < a; i++){
    //     for(int j = 0; j < c; j++){
    //         for(int k = 0; k < b; k++){
    //             result[i*c + j] += _m[i*b +k] * right_vec[k*c + j];
    //         }
            
    //     }
    // }

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

Matrix * Matrix::shallowCopy(){
    Matrix* mx = new Matrix(_m, _num_row, _num_column);
    return mx;
}

Matrix * Matrix::deepCopy(){
    double* _m_copy = new double[_num_row*_num_column];
    for(int i = 0 ; i < _num_row*_num_column; i++){
        _m_copy[i] = _m[i];
    }
    Matrix* mx = new Matrix(_m, _num_row, _num_column);
    return mx;
}

void Matrix::add(Matrix* right){
    if(right->_num_row != _num_row || right->_num_column != _num_column){
        cout<<"Wrong addition!\n";
        exit(1);
    }

    for(int i = 0; i <_num_row; i++){
        for(int j = 0; j <_num_column; j++){
            _m[i*_num_column + j] += right->_m[i*_num_column + j];
        }
    }
}

void Matrix::minus(Matrix* right){
    if(right->_num_row != _num_row || right->_num_column != _num_column){
        cout<<"Wrong minus!\n";
        exit(1);
    }

    for(int i = 0; i <_num_row; i++){
        for(int j = 0; j <_num_column; j++){
            _m[i*_num_column + j] -= right->_m[i*_num_column + j];
        }
    }
}


void Matrix::minusedby(Matrix* left){
    if(left->_num_row != _num_row || left->_num_column != _num_column){
        cout<<"Wrong minus!\n";
        exit(1);
    }

    for(int i = 0; i <_num_row; i++){
        for(int j = 0; j <_num_column; j++){
            _m[i*_num_column + j] = left->_m[i*_num_column + j] - _m[i*_num_column + j];
        }
    }
}


void Matrix::minusScalar(Matrix* right, int start, int end){
    if(_num_row != end - start  || _num_column != 1){
        cout<<"Wrong minus scalar!\n";
        exit(1);
    }

    for(int i = 0; i < end - start; i++){
        _m[i] -= right->_m[start + i];
    }
}

double Matrix::trace(){
    if(_num_row  != _num_column){
        cout<<"Wrong trace!\n";
        exit(1);
    }

    double result = 0;
    for(int i = 0; i <_num_row; i++){
        result += _m[i*_num_column + i];
    }
    return result;
}

double Matrix::toDouble(){
    if(_num_row  != 1 || _num_column != 1){
        cout<<"Wrong to double!\n";
        exit(1);
    }

    return _m[0];
}

void Matrix::divide(double v){
    for(int i = 0; i <_num_row; i++){
        for(int j = 0; j <_num_column; j++){
            _m[i*_num_column + j] /= v;
        }
    }
}

void Matrix::TransposeOne(){
    if(_num_row != 1 && _num_column != 1){
        cout<<"wrong transpose one!\n";
        exit(1);
    }
    if(_num_row == 1){
        _num_row = _num_column;
        _num_column = 1;
    }
    else{
        _num_column = _num_row;
        _num_row = 1;
    }
    
}

void Matrix::inverse(){
    if(_num_row  != _num_column){
        cout<<"Wrong inversion!\n";
        exit(1);
    }

    int IPIV[_num_row];
    int INFO=10;
    int LWORK = _num_row*_num_row;

    double* b = new double[_num_row *_num_row ];

    dgetrf_(&_num_row,&_num_row,_m,&_num_row,IPIV,&INFO);
    dgetri_(&_num_row,_m,&_num_row,IPIV,b,&LWORK,&INFO);

    delete[] b;
}

void Matrix::printSelf(){
    for(int i = 0; i < _num_row ; i++){
        for(int j = 0; j <  _num_column; j++){
            cout << fixed << setprecision(2) << _m[i* _num_column + j] <<" ";
        }
        cout << "\n";
    }
}

void Matrix::printShape(){
    cout<<"height: " << _num_row <<" width: " << _num_column <<"\n";
}